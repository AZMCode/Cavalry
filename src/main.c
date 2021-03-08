#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <wchar.h>
#include <utils.h>
#include <mod.h>

int main(){
    wprintf(L"%s",stringifyBoard(&defaultBoard));
}