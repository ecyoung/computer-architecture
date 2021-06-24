.text
.align 2

main: # main function
li $s0, 19  
li $t0, 1

li $v0, 4 
la $a0, prompt 
syscall 

# read input number
li $v0, 5 
syscall 
move $s1, $v0

loop: # enter loop
mult $s0, $t0
mfhi $t1
mflo $t2 
li $v0, 1 
move $a0, $t2
syscall # print $t2
li $v0, 4
la $a0, nln
syscall # print empty line
addi $t0, $t0, 1
bgt $t0, $s1, exit
j loop # jump to loop

exit: 
li $v0, 10
syscall # exit loop

.data 
prompt:
.asciiz "Input a number:\n" 
nln:      
.asciiz "\n"