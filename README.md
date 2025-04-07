# Informace o projektu #
V následující složce **__Velka_uloha__** je vypracování progtestové velké úlohy. Jedná se o semestrální úlohu vypracovanou v předmětu BI-PA2. Představuje výpočetní logiku programů typu Excel, kdy lze na jednotlivé buňky [A-Z...][0-999...] uložit číslo, řetězec nebo výraz za znakem "=", který se pak efektivně reprezentuje v paměti pomocí AST (_Abstract Syntax Tree_) polymorfní struktury. 

Celou tabulku lze pak uložit do souboru, jejíž obsah se uloží v šestnáctkové (hexadecimální) soustavě a lze z takových souborů opět načíst a dále s takovou tabulkou pracovat. Úlohu jsem vypracoval za pomoci Visual Studio Code editoru a CMake extension - proto jsou také v repozitáři _build_ složka a CMakeLists.txt. K vypracování jsem použil MinGW32 parser, který dokázal vzít výraz a z infixové podoby typu "4 + A2 + B$7" vytvořil postfixovou podobu "4 A2 + B$7 +". Tohle ale nevytváří explicitně a místo toho volá specifické metody valNumber(4), valReference("A2"), opAdd(), valReference("B$7"), opAdd() -- jejichž implementace byla klíčová pro korektní vytvoření AST datové struktury.

**Test.cpp** je hlavní souborem se základními testy. **Headers** je složka se všemi hlavičkovými soubory, do kterých jsem rozdělil ty podstatnější třídy. 

V _Literals.hpp_ jsou všechny třídy, které odpovídají literálům ve výrazu. V _Operators.hpp_ jsou všechny třídy odpovídající operátorům ve výrazu.

**Implementations** je složka se všemi hlavičkovými implementacemi tříd ze složky Headers.

**All_in_one.cpp** je na Progtest odevzdaný zdrojový kód, který prošel základními testy a nepovinným testem cyklických závilostí (pomocí upraveného DFS).
