#!/usr/bin/env sh

function read_includes()
{
    grep include $1 | grep ROOT_DIR | grep -v scripts | while IFS= read -r line; do
        j=`echo "$line" | sed "s:\/: :g" | awk '{print $(NF-1)}'`
        f=`echo "$line" | sed "s:\/: :g" | awk '{for (i=3; i<=NF; i++) print $i}'`
        f=`echo $f | sed "s: :\/:g"`
        status="on"
        if [ ${line:0:1} == "#" ]; then status="off"; fi
        echo "$j $f $status"
    done
}

function write_includes()
{
    fname=$1
    shift
    while IFS= read -r line; do
        if [[ $line == *"include"*"ROOT_DIR"* ]]; then
            i=`echo "$line" | awk '{print $NF}'`
            j=`echo "$line" | sed "s:\/: :g" | awk '{print $(NF-1)}'`;
            if [[ $@ =~ $j ]]; then
                echo "include $i"
            else
                echo "#include $i"
            fi
        else
            echo "$line"
        fi
    done < $fname
}

function find_index()
{
    for i in `seq 1 1 $N`; do
        j=`echo $items2 | cut -d " " -f $i`
        k=${j:0:1}
        if [ $k == "$1" ] || [ ${k^} == "$1" ]; then
            echo $i
            exit
        fi
    done
}

function get_selection()
{
    n=1
    while IFS= read -r line; do
        if [[ $line == *"DLGK_ITEM_NEXT"* ]]; then
            n=$((n + 1))
        fi
        if [[ $line == *"DLGK_ITEM_PREV"* ]]; then
            n=$((n - 1))
        fi
        if [[ $line == *"DLGK_ITEM_FIRST"* ]]; then
            n=0
        fi
        if [[ $line == *"DLGK_ITEM_LAST"* ]]; then
            n=$N
        fi
        if [[ $line == "chr "?" "* ]]; then
            i=`find_index ${line:4:1}`
            if [[ ! -z  $i ]]; then
                n=$i
            fi
        fi
        if (( n > N )); then
            n=$N
        fi
        if (( n < 0 )); then
            n=0
        fi
    done < $1
    echo $n
}

function get_selection_path()
{
    fname=$1
    item=$2
    j=`grep include $fname | grep $item`
    k=`echo "$j" | sed "s:\/: :g" | awk '{for (i=3; i<=NF; i++) print $i}'`
    mk=`echo $k | sed "s: :\/:g"`
    n=`read_includes $mk | wc -l`
    if [ $n != "0" ]; then
        echo $mk
    fi
}

function edit_includes()
{
    items=`read_includes $1`
    #items=`echo "$a" | awk '{print $1, $1, $2}'`
    items2=`echo "$items" | awk '{print $1}'`
    N=`echo $items2 | wc -w`

    tempfile=`tempfile 2>/dev/null` || tempfile=/tmp/stmtmp$$
    trap "rm -f $tempfile" 0 1 2 5 15
    result=$(dialog --trace $tempfile --ok-label Select --extra-button --extra-label Save --cancel-label Exit --checklist "$1" 30 50 23 $items 3>&1 1>&2 2>&3 3>&-)
    exitcode=$?

    n=`get_selection $tempfile`

    selection=`echo $items2 | cut -d " " -f $n`

    echo $exitcode $selection $result
}

mkf=Makefile
path="."

while true
do
    ret=`edit_includes $path/$mkf || exit 0`
    items=`echo $ret | awk '{for (i=3; i<=NF; i++) print $i}'`
    ret=($ret)

    case "${ret[0]}" in
        0)
            echo Select
            if [ $path == "." ] && [ $mkf == Makefile ]; then
                path="${ret[1]}"
            else
                f=`get_selection_path $path/$mkf ${ret[1]}`
                if [ -e "$f" ]; then
                    path=`dirname $f`
                    mkf=`basename $f`
                fi
            fi
            ;;
        3)
            echo Save
            tempfile=`tempfile 2>/dev/null` || tempfile=/tmp/stmtmp$$
            write_includes $path/$mkf $items scripts > $tempfile
            mv $tempfile $path/$mkf
            ;;
        1)
            echo Exit
            if [ $mkf == Makefile ]; then
                if [ $path == "." ]; then
                    clear
                    exit 0
                else
                    path="."
                fi
            else
                echo $path
                p=`dirname $path`
                if [ $p == "." ]; then
                    mkf=Makefile
                else
                    path=$p
                    mkf=`basename $path`.mk
                fi
            fi
            ;;
    esac
done

