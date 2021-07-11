
find . -name "*.h" -o -name "*.cpp" > cscope.files
sort "cscope.files" > "cscope.files.sorted"
mv "cscope.files.sorted" "cscope.files"

cscope -q -R -b -i "cscope.files"
ctags -L "cscope.files"
