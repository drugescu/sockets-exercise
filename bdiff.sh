rm -rf $1.bin
rm -rf received_$1.bin
xxd $1 > $1.bin
xxd received_$1 > received_$1.bin
diff $1.bin received_$1.bin
