# Injectah

## Overview
This is a simple injector made for productivity with an entry in the context menu and 
an .ini file with the list of processes one wishes to inject to.  
In case no listed processes are open you get a msgbox telling you so, in case more than
one is open, you get a box with the various options.  
The point of this is not to be the best injector all around the world, but to make work
faster, since testing dlls with a regular injector might get boring and slow due to the
amount of times one has to inject.  
## Set Up
Fairly Easy To Setup:
Put Injectah.exe into a folder, create an injectah.ini(example in examples folder) with 
the process names you wantthe dll to be injected to, and execute RegisterMenu.bat in that 
same folder. Profit.
## Methods Of Injection
#### Default
I did not include the file containing the inject functions *LoadLibraryInject* and 
*ThreadHijackInject* that is *"Inject.h"* since there are already many options out there 
for anyone looking to copy paste, and anyone at least somewhat smart can or has made their 
own.  
#### Add Your Own
It's not hard to add your own functions, in *main.cpp* there are the defines in line 12-13,
the read (command line arguments) in line 95-97, and the check on line 212 onward.
Then all you have to do is add an entry to RegisterMenu.bat, which you'd do by adding  
```
@reg add "HKEY_CLASSES_ROOT\dllfile\shell\Inject\Shell\cmdN"         /t REG_SZ /v "MUIVerb" /d "Option Name"   /f
@reg add "HKEY_CLASSES_ROOT\dllfile\shell\Inject\Shell\cmdN\command" /t REG_SZ /v "" /d "\"%CD%\Injectah.exe\" \"%%1\" /CmdArg" /f
```
where ``n`` in cmdN is the index, ``Option Name`` the type of injection and ``CmdArg`` the 
letter that's gonna show up in said line 95-97 from *main.cpp*, to the end of *RegisterMenu.bat*  

## Me
Feel free to leave feedback, this was my first post on github and is probably complete trash,
chances are I even was a dick somehow by doing something that isn't generally accepted in
this community as I do with most starting out.
So ye, you can even be a dick about it, as long as it's constructive criticism.
