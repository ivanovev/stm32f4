#!/usr/bin/env sh

mk=`ls | grep mk$ | head -n 1`
mkf=Makefile

function read_includes()
{
    grep include $mkf | grep ROOT_DIR | grep -v scripts | grep -v $mk | while IFS= read -r line; do
        j=`echo "$line" | sed "s:\/: :g" | awk '{print $(NF-1)}'`;
        status="on";
        if [ ${line:0:1} == "#" ]; then status="off"; fi
        echo "$j $status"
    done
}

function write_includes()
{
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
    done < $mkf
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

a=`read_includes`
items=`echo "$a" | awk '{print $1, $1, $2}'`
items2=`echo "$a" | awk '{print $1}'`
N=`echo $items2 | wc -w`

tempfile=`tempfile 2>/dev/null` || tempfile=/tmp/stmtmp$$
#trap "rm -f $tempfile" 0 1 2 5 15
result=$(dialog --trace $tempfile --checklist "$mkf" 30 50 23 $items 3>&1 1>&2 2>&3 3>&-);# 2> $tempfile || exit 0)
exitcode=$?;

echo $result
n=0
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
done < $tempfile
cp $tempfile /tmp/2
rm -rf $tempfile

echo $n $N

tempfile=`tempfile 2>/dev/null` || tempfile=/tmp/stmtmp$$
write_includes $result scripts > /tmp/1 #$tempfile
#cp $tempfile $mkf
#clear

