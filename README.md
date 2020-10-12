# Secret-Santa

A tool to assist in the secret santa process without much hassle.

Execute either the .exe executable OR compile and run the main.cpp file yourself.

Add the amount of people participating in the Secret Santa process (more than 2 people and of course different names).

After you enter each name, the same amount of files will be created in the respective directory where the program was executed. Each file has the name of a person who particiapted and inside each file is the name of the person whom they are going to get a gift for. Simply send the file without opening it to its respective owner and only open the file with your name in.

-------------------------------------------------------How it works------------------------------------------------------------

The process works by creating 2 string arrays with all the names, then creating a file for each cell in the first array and writing a random name in that file from the second array. The name is then deleted from the second array and the process continues until everyone has been assigned a secret santa!

The default file format is .txt but that can change from the core file if you want. Enjoy!
