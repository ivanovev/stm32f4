#!/usr/bin/env sh

function read_includes()
{
    grep include $1 | grep ROOT_DIR | grep -v scripts | while IFS= read -r line; do
        j=`echo "$line" | sed "s:\/: :g" | awk '{print $(NF-1)}'`;
        status="on";
        if [ ${line:0:1} == "#" ]; then status="off"; fi
        echo "$j $status"
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
    i=`grep include $fname | grep $item`
    echo $i
}

function edit_includes()
{
    a=`read_includes $1`
    items=`echo "$a" | awk '{print $1, $1, $2}'`
    items2=`echo "$a" | awk '{print $1}'`
    N=`echo $items2 | wc -w`

    tempfile=`tempfile 2>/dev/null` || tempfile=/tmp/stmtmp$$
    #trap "rm -f $tempfile" 0 1 2 5 15
    result=$(dialog --trace $tempfile --ok-label Select --extra-button --extra-label Exit --cancel-label Save --checklist "$1" 30 50 23 $items 3>&1 1>&2 2>&3 3>&-);# 2> $tempfile || exit 0)
    exitcode=$?;

    #echo $result
    n=`get_selection $tempfile`
    cp $tempfile /tmp/2
    rm -rf $tempfile

    selection=`echo $items2 | cut -d " " -f $n`

    tempfile=`tempfile 2>/dev/null` || tempfile=/tmp/stmtmp$$
    write_includes $1 $result scripts > /tmp/1 #$tempfile
    #cp $tempfile $1
    #clear
    echo $exitcode $selection $items2 $n $N
}

mkf=Makefile
path="."

while true
do
    ret=`edit_includes $path/$mkf || exit 0`
    ret=($ret)

    case "${ret[0]}" in
        0)
            echo Select
            if [ $mkf == Makefile ]; then
                if [ $path == "." ]; then
                    path="${ret[1]}"
                    echo $path
                    read
                else
                    mkf="$(path).mk"
                fi
            else
                path="$path/${ret[1]}"
                mkf="${ret[1]}.mk"
            fi
            ;;
        1)
            echo Save
            ;;
        3)
            echo Exit
            get_selection_path $path/$mkf ${ret[1]}
            read
            if [ $path == "." ] && [ $mkf == Makefile ]; then
                if [ $path == "." ]; then
                    clear
                    exit 0
                else
                    path="."
                fi
            else
                path=`dirname $path`
                mkf=`basename $path`.mk
            fi
            ;;
    esac
done

