# Info #
V následující složce **__Velka_uloha__** je vypracování progtestové velké úlohy. Úlohu jsem vypracoval za pomoci Visual Studio Code editoru a CMake extension - proto jsou také ve složce Build složka a CMakeLists.txt. K vypracování jsem použil MinGW32 parser, původně jsem ale pracoval na MinGW64, takže bylo potřeba projekt začít odznova a zkopírovat rozpracované soubory (proto je jeden z commitů Created identical project ...).

**Test.cpp** je hlavní souborem se základními testy. **Headers** je složka se všemi hlavičkovými soubory, do kterých jsem rozdělil ty podstatnější třídy. 

V _Literals.hpp_ jsou všechny třídy, které odpovídají literálům ve výrazu. V _Operators.hpp_ jsou všechny třídy odpovídající operátorům ve výrazu.

**Implementations** je složka se všemi hlavičkovými implementacemi tříd ze složky Headers.

**All_in_one.cpp** je na Progtest odevzdaný zdroják, který prošel základními testy a nepovinným testem cyklických závilostí (pomocí upraveného DFS).



# Command line instructions #

You can also upload existing files from your computer using the instructions below.

**Git global setup** 
```
git config --global user.name "Jakub Černý"
git config --global user.email "cernyj90@fit.cvut.cz"
```
**Create a new repository**
```
git clone git@gitlab.fit.cvut.cz:BI-PA2/b232/BI/cernyj90.git
cd cernyj90
git switch --create main
touch README.md
git add README.md
git commit -m "add README"
git push --set-upstream origin main
```
**Push an existing folder**
```
cd existing_folder
git init --initial-branch=main
git remote add origin git@gitlab.fit.cvut.cz:BI-PA2/b232/BI/cernyj90.git
git add .
git commit -m "Initial commit"
git push --set-upstream origin main
```
**Push an existing Git repository**
```
cd existing_repo
git remote rename origin old-origin
git remote add origin git@gitlab.fit.cvut.cz:BI-PA2/b232/BI/cernyj90.git
git push --set-upstream origin --all
git push --set-upstream origin --tags
```
