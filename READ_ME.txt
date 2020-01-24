Austin Tsao and Michelle Wen
README for "cheaters_mw37583.zip"

1.  In directory of your choice, run "unzip cheaters_mw37583.zip" to unzip files
2. After unzipping, type "make" to compile and link files
3.  Run command "./plagiarismCatcher path/to/text/files (number of words per chunk) (number of similarities detected)" to run program.

For example: 
"./plagiarismCatcher sm_doc_set 6 200"
This will run the program on the directory "sm_doc_set". The word chunk size will be set at 6 words.  Only those files with 200 and above similarities will be displayed on the terminal.

Example output line:
370: tyc12.txt, catchmeifyoucan.txt
-> This means that 370 similarities were found between "tyc12.txt" and "catchmeifyoucan.txt"

Notes:
The program will output the number of similarities detected as well as the 2 files similarities were detected in.  This will display in descending order (ie the most plagiarized files at the top, least plagiarized files at the bottom).  The program will only display those files with similarities detected above the minimum # of similarities detected that is inputted to the command terminal. 

Though valgrind errors exist, no memory leaks are detected. Flannery said to ignore valgrind errors since there were no memory leaks and the output looked good.