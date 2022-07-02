# Copy-Dirs-And-Files
Use threading to copy a source directory and its contents to a destination directory.

Write a C/C++ code to copy the content of one directory to another directory. Your program should take as input two paths: a path to a source directory and a path to the destination directory. Your code should satisfy the followings:

1.You should use three processes: the parent and two children.

2.Initially, the parent scans the source directory for files and sub-directories, and compute the total size of the files in this directory. The parent then creates two children to copy the specified directory. The parent should divide the tasks as follows:

•Find the median of the file sizes.

•One of the two children will copy all the files smaller the median and the other will copy all files larger than the median.

•The copied files will have the same permissions as the original files.

•The hierarchy of the source and the destination directory should be the same.

Do Not Forget:

•Check for errors.

•Parent must wait for children.
