.text
.align 2

main: # main function
	addi $sp, $sp, -8
	sw $ra, 0($sp)

	li $v0, 4 
	la $a0, prompt 
	syscall 

	# read input number
	li $v0, 5 
	syscall 
	move $t0, $v0 # copy N to $t0

	# call recursive function f(N)=f(N-1)+2*f(N-2)+3
	move $a0, $t0   # argument
	move $v0, $t0
	sw $t0, 4($sp) # save $t0 on stack

	jal recurse     # jump and link to recurse
	lw $t0, 4($sp)
	move $t1, $v0   # result in $t1

	# output 
	move $a0, $t1   # print answer
	li $v0, 1
	syscall

	la $a0, nln # print empty line
	li $v0, 4
	syscall

	# exit program
	lw $ra, 0($sp)
	addi $sp, $sp, 8
	jr $ra
	# li $v0, 10 
	# syscall 

recurse:
	addi $sp,$sp,-16 # make frame 
	sw $ra, 0($sp)
	sw $s0, 4($sp)
	sw $s1, 8($sp)
	sw $s2, 12($sp)

	move $s0, $a0 # take argument and store it to register $s0

	beq $s0, 0, zero # branch to zero if input is 0
	beq $s0, 1, one # branch to one if input is 1

	addi $a0, $s0, -1 # f(N-1)

	jal recurse

	move $s1, $v0 # store return value in $s1
	addi $a0, $s0, -2 # f(N-2)

	jal recurse
	# Do the math: f(N)=f(N-1)+2*f(N-2)+3
	li $s2, 2
	mult $s2, $v0 # 2*f(N-2) 
	mflo $v0
	add $v0, $s1, $v0 # f(N-1)+2*f(N-2)
	addi $v0, $v0, 3  # f(N-1)+2*f(N-2)+3

exit: # load and restore
	lw $ra, 0($sp) # load registers from stack
	lw $s0, 4($sp)
	lw $s1, 8($sp)
	lw $s2, 12($sp)
	addi $sp, $sp, 16 # restore stack pointer
	jr $ra

zero: # base case for N = 0
	li $v0, 0
	j exit
one: # base case for N = 1
	li $v0, 1
	j exit

.data
prompt: 
.asciiz "Input a number:\n"
nln: 
.asciiz "\n"