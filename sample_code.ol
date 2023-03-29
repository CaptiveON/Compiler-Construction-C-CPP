calculateFib: int <- function($n: int){
	#~variable declarations, note z is not initialized~#
    $x:int = 0;
    $y:int = 0;
    $z:int;

    if ($n -lt 0) then {
        displayline:'negative number entered';
        return:0;
    } 
    else if ($n -eq 0) then {
        displayline:"it's 0 man!";
        return:0;
    } 
    else {
        display:"\nThe fibonacci series: ";
        $i:int = 1;
        do until($i-1 -ge $n){
            display:$x," ";
            $z = $x + $y;
            $x = $y;
            $y = $z;
            $i = $i + 1;
        }
    }
    return:0;
}

#~ program to calculate fibonacci series and display on stdout ~#
main: int <- function(){
    $num: int;
    "Enter the number" :read: $num; #~ input ~#
    $result: int = execute: calculateFib($num,0);
    return:0; #~halt~#
}