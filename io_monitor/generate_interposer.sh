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

LINE_CTR=1

# automatic part of generated files
cat monitored_functions.data | while read -r LINE ; do
    LINE_CTR=$((LINE_CTR+1))
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
    echo input line: $LINE_CTR
    echo return: $RET
    echo name: $NAME
    echo "*/"

    echo $PROTOTYPE
    echo '{'
    echo 'CHECK_LOADED_FNS();'
    echo 'PUTS("'$NAME'");'
cat <<EOF
    DECL_VARS();
   GET_START_TIME();
EOF
    # only variadic call for now is open and its friends 
    echo $PROTOTYPE | grep -F '...' >/dev/null
    VAU=$? # var args usage
    echo $PROTOTYPE | grep -F 'open' >/dev/null
    ISOPEN=$? # some variant of open (so mode can be passed in last argument
    
    if [ $VAU -eq 0 ] ; then
	echo "   /* set up variadic arguments */"
	echo 'va_list args;'
	echo -n 'va_start(args, '
	echo $PROTOTYPE |  rev | cut -d ',' -f 2 | cut -d ' ' -f 1  | rev | tr '\n' ')'
	echo ';'
	if [ $ISOPEN -eq 0 ] ; then
	    echo "   /* extract mode for open call */"
	    echo 'int mode = va_arg(args, int);'
	fi
    fi
    echo  "\n   /* call original function */"
    if [ $VAU -eq 0 ] && [ $ISOPEN -ne 0 ] ; then
	echo -n "   $RET result = orig_v$NAME("
    else
	echo -n "   $RET result = orig_$NAME("
    fi
    echo $PROTOTYPE | cut -d '(' -f 2- | tr ',' '\n'|
	while read -r ARG ; do
	    echo $ARG | grep -F '...' >/dev/null
	    if [ $? -eq 0 ] ; then
		if [ $ISOPEN -eq 0 ] ; then
		    echo -n mode\),
		else
		    echo -n args\),
		fi
	    else
		echo $ARG | rev | cut -d ' ' -f 1  | rev | tr '\n' ','
	    fi
	done | sed 's/),/)/'
    echo ';'
    if [ $VAU -eq 0 ] ; then
	echo 'va_end(args);'
    fi
    echo '   GET_END_TIME();'
    #if hook or prototype contains variable fd
    echo $HOOK_AFTER $PROTOTYPE | grep -F ' fd' >/dev/null
    if [ $? -eq 0 ] ; then
	FD='fd'
    else
	FD='FD_NONE'
    fi
    echo $HOOK_AFTER $PROTOTYPE | grep -F ' count' >/dev/null
    if [ $? -eq 0 ] ; then
	COUNT=count
    else
	COUNT=ZERO_BYTES
    fi
    echo "\n   /* invoke hook */"
    echo "   $HOOK_AFTER"
    echo "   /* end of hook; record metadata on function call */"
    echo "   record($DOMAIN, $OP, $FD, $S1, $S2, "
    echo "   TIME_BEFORE(), TIME_AFTER(), error_code, $COUNT);"
    echo "   return result;"
    echo "}"
    ) | indent >> intercept_functions.h
    echo >> intercept_functions.h
    
    echo typedef $RET \(*orig_${NAME}_f_type\) \(`echo $PROTOTYPE | cut -d '(' -f 2-`\; >> io_function_types.h

    echo static orig_${NAME}_f_type orig_$NAME = NULL\; >> io_function_orig_handlers.h >> io_function_orig_handlers.h

    echo orig_$NAME = \(orig_${NAME}_f_type\)dlsym\(RTLD_NEXT,\"$NAME\"\)\; >> assign_functions.h

done 

echo } >> assign_functions.h
