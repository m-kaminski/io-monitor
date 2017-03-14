#!/bin/sh

#generate contents of following files:
# io_function_types.h - typedefs of functions that are intercepted
rm -f io_function_types.h
# io_function_orig_handlers.h - definitions of function pointers adhering to typedefs listed above
rm -f io_function_orig_handlers.h
# intercept_functions.h - intercept functions themselves
rm -f intercept_functions.h
# assign_functions.h - assigns original functions 
rm -f assign_functions.h

which indent  >/dev/null
if [ $? -ne 0 ] ; then
    echo "Command indent not found. Install it with apt-get install indent before proceeding"
    exit 1;
fi
# headers of generated files

echo 'void assign_lib_functions()' >> assign_functions.h
echo \{ >> assign_functions.h

# automatic part of generated files
cat monitored_functions.data | while read -r LINE ; do
    if echo $LINE | egrep '^#|^$' >/dev/null ; then
	continue;
    fi

    PROTOTYPE=`echo $LINE | cut -d '|' -f 1`
    RET_AND_NAME=`echo $PROTOTYPE | cut -d '(' -f 1`
    NAME=`echo $RET_AND_NAME | rev|  cut -d ' ' -f '1' | rev`
    RET=`echo $RET_AND_NAME | rev |cut -d ' ' -f '1' --complement | rev`

    DOMAIN=`echo $LINE | cut -d '|' -f 2`
    OP=`echo $LINE | cut -d '|' -f 3`
    S1=`echo $LINE | cut -d '|' -f 4`
    S2=`echo $LINE | cut -d '|' -f 5`
    HOOK_AFTER=`echo $LINE | cut -d '|' -f 6`
    REALPATH=`echo $LINE | cut -d '|' -f 7`

    #generate content for intercept_functions.h
    (
    echo "/*"
    echo return: $RET
    echo name: $NAME
    echo "*/"

    echo $PROTOTYPE
    echo '{'
    cat <<EOF
   CHECK_LOADED_FNS();
   PUTS("open");
   DECL_VARS();
   GET_START_TIME();
EOF
    # only variadic call for now is open and its friends 
    echo "int mode = 0;"
    echo -n "   $RET result = orig_$NAME("
    echo $PROTOTYPE | cut -d '(' -f 2- | tr ',' '\n'|
	while read -r ARG ; do
	    echo $ARG | grep -F '...' >/dev/null
	    if [ $? -eq 0 ] ; then
		echo -n mode\),
	    else
		echo $ARG | rev | cut -d ' ' -f 1  | rev | tr '\n' ','
	    fi
	done | sed 's/),/)/'
    echo ';'

    echo $PROTOTYPE | grep -F ' fd' >/dev/null
    if [ $? -eq 0 ] ; then
	FD='fd'
    else
	FD='0'
    fi
    echo "   $HOOK_AFTER"
    echo "   record($DOMAIN, $OP, $FD, $S1, $S2, "
    echo "   TIME_BEFORE(), TIME_AFTER(), error_code, ZERO_BYTES);"
    echo "   return result;"
    echo "}"
    ) | indent >> intercept_functions.h

    echo typedef $RET \(*orig_${NAME}_f_type\) \(`echo $PROTOTYPE | cut -d '(' -f 2-`\; >> io_function_types.h

    echo static orig_${NAME}_f_type orig_$NAME = NULL\; >> io_function_orig_handlers.h >> io_function_orig_handlers.h

    echo orig_$NAME = \(orig_${NAME}_f_type\)dlsym\(RTLD_NEXT,\"$NAME\"\)\; >> assign_functions.h

done 

echo } >> assign_functions.h
