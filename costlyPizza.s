.text 
.align 2 

main: # main function (caller function)
# save return address
	addi $sp, $sp, -4
	sw $ra, 0($sp)
# allocate memory space for initial node
	li, $a0, 72 # string: 64 bytes, float: 4 bytes, next: 4 bytes
	li $v0, 9
	syscall 
	move $s0, $v0 # save linked list head in $s0
# prompt name
	li $v0, 4 
	la $a0, promptName
	syscall 
# read name
	move $a0, $s0 # copy linked list address to $a0
	li $v0, 8 
	li $a1, 64 
	move $s1, $a0
	syscall
# compare the input to "DONE"
	la $a2, done # pass address of "DONE"
	jal strcmp  # call strcmp
	beq $v0, 0, exit # if the first input = "DONE", exit program
# store diameter
	li $v0, 4 
	la $a0, promptDiameter
	syscall 

	li $v0, 6		
	syscall			
	mov.s $f1, $f0	
# store cost
	li $v0, 4 
	la $a0, promptCost
	syscall 

	li $v0, 6
	syscall 
	mov.s $f2, $f0
# make calculations for cost/area
	li.s $f3, 0.25
	# area = 0.25 * PI * (diameter*diameter)
	li.s $f4, 3.14159265358979323846 # PI
	mul.s $f5, $f3, $f4 # 0.25 * PI
	mul.s $f6, $f1, $f1 # diameter * diameter
	mul.s $f7, $f5, $f6 # 0.25 * PI * (diameter*diameter)
	# if (diameter != 0) calculation = cost/area
	li.s $f8, 0.0
	c.eq.s $f1, $f8 # set flag if $f1 == $f8, else clear it 
	bc1t zeroHead # diameter = 0 -> branch to zeroHead
	div.s $f9, $f2, $f7 # calculation = cost/area
	s.s $f9, 64($s0)

# add nodes to linked list
insert:
	move $s2, $s0 # store first node for later use
	li $a0, 72
	li $v0, 9
	syscall
	move $s3, $v0 # link nodes
# store name
	li $v0, 4 
	la $a0, promptName
	syscall 

	li $v0, 8 
	move $a0, $s3 # copy linked list address to $a0
	li $a1, 64 
	syscall
# compare the input to "DONE"
	la $a2, done # pass address of "DONE"
	jal strcmp  # call strcmp
	beq $v0, 0, print # if the input = "DONE", print linked list
# store diameter
	li $v0, 4 
	la $a0, promptDiameter
	syscall 

	li $v0, 6		
	syscall			
	mov.s $f1, $f0	
# store cost
	li $v0, 4 
	la $a0, promptCost
	syscall 

	li $v0, 6
	syscall 
	mov.s $f2, $f0
# make calculations for cost/area
	li.s $f3, 0.25
	# area = 0.25 * PI * (diameter*diameter)
	li.s $f4, 3.14159265358979323846 # PI
	mul.s $f5, $f3, $f4 # 0.25 * PI
	mul.s $f6, $f1, $f1 # diameter * diameter
	mul.s $f7, $f5, $f6 # 0.25 * PI * (diameter*diameter)
	# if (diameter != 0) calculation = cost/area
	li.s $f8, 0.0
	c.eq.s $f1, $f8 # set flag if $f1 == $f8, else clear it 
	bc1t zeroRest # diameter = 0 -> branch to zeroRest
	div.s $f9, $f2, $f7 # calculation = cost/area
	s.s $f9, 64($s3)

	li $v0, 1 # set return value counter to 1 

loop: # add contents to linked list with insertion sort 
	beq $v0, 1, afterfirstPizza # $v0 == 1
	lw $s4, 68($s2) # next
	beq $s4, 0, last # branch to last if "next" is empty
	bne $s4, 0, notLast # branch to notLast if "next" is not empty

	afterfirstPizza: 
	l.s $f10, 64($s0) # load cost/area into the node after the first node
	li $v0, 0 # temporary node 
	c.lt.s $f10, $f9 # $f9 > $f10
	bc1t firstPizzaGreater
	c.eq.s $f10, $f9 # $f9 = $f10
	bc1t equalFirstPizza
	j loop

	equalFirstPizza: # first two pizzas have the same stats
	la $a0, 0($s0)
	la $a1, 0($s3)
	jal alphComp
	beq $v1, 1, firstPizzaGreater
	j loop

	firstPizzaGreater: # first pizza is greater than second pizza
		sw $s2, 68($s3)
		move $s0, $s3
		j insert

	last: 
		sw $s3, 68($s2)
		move $s2, $s0
		j insert 

	notLast:
		l.s $f10, 64($s4) # save cost/area for next node 
		c.lt.s $f10, $f9  # check if next < current 
		bc1f notGreater

		j greater

	greater: 
	sw $s3, 68($s2)
	sw $s4, 68($s3)  
	move $s2, $s0
	j insert

	notGreater:
		c.eq.s $f9, $f10 # check if current == next
		bc1f notEqual
		j equal

	equal: 
		move $a0, $s3 # copy current to $ao
		move $a1, $s4 # copy next to $a0
		jal alphComp
		beq $v1, 0, greater
		lw $s2, 68($s2)
		j loop

	notEqual:
		lw $s2, 68($s2)
		j loop

zeroHead: # sets the cost of the first pizza to 0
	s.s $f8, 64($s0)
	j loop 

zeroRest: # sets the cost of the rest of pizzas to 0 
	li $v0, 1
	mov.s $f9, $f8
	s.s $f8, 64($s3)
	j loop

print: # print out name and costs of pizzas
	la $a0, 0($s0) 
	jal trim # call trim
	la $a0, 0($s0) 
    li $v0, 4 
    syscall

   	li $a0, 32 # ASCII number for space
	li $v0, 11  # syscall number for printing character
	syscall

	li $v0, 2 # print cost/area
  	l.s $f12, 64($s0)
  	syscall

  	li $v0, 4 # print empty line
	la $a0, nln
	syscall 

	lw $s0, 68($s0)
	beq $s0, 0, exit

	j print

strcmp:
	addi $sp, $sp, -4 # make frame 
	sw $ra,0($sp)

	add $t0, $zero, $a0  # load input into $t0
	add $t1, $zero, $a2 # load "DONE" into $t1
	cmp:
		lb $t2, 0($t0) # load a byte from $t0 and store it to $t2
		lb $t3, 0($t1) # load a byte from $t1 and store it to $t3
		beq $t2, 0, same # input ends
		slt $t4, $t2, $t3  # set $t4 to 1 if $t2 is less than $t3
		slt $t4, $t3, $t2 # set $t4 to 1 if $t3 is less than $t2
		bne $t4, 0, different 
		addi $t0, $t0, 1 # points to the next byte in $t0
		addi $t1, $t1, 1 # points to the next byte in $t1
		j cmp 

	different: 
		li $v0, 1 # 1 represents the input is not "DONE"
		j end

	same:
		li $v0, 0 # 0 represents the input is "DONE"

	end:
		lw $ra, 0($sp)
  		addi $sp, $sp, 4
		jr $ra

alphComp: # sort alphabetically 
	addi $sp, $sp, -24 # make frame 
	sw $ra, 0($sp) # save registers on stack
	sw $s0, 4($sp)
	sw $s1, 8($sp)
	sw $s2, 12($sp)
	sw $s3, 16($sp)
	sw $s4, 20($sp)

	move $s0, $a0
	move $s1, $a1

	loopComp:
		lb $s2, 0($s0) # $s2 loads the bytes of current pizza
		lb $s3, 0($s1) # $s3 loads the bytes of next pizza
		slt $s4, $s2, $s3 # set flag if $s2 < $s3
		beq $s4, 1, less

		notLess:
			slt $s4, $s3, $s2 # check if $s2 > $s3
			beq $s4, 1, more 
			beq $s4, 0, increment 

		less: 
			li $v1, 0
			lw $ra, 0($sp)
			lw $s0, 4($sp)
			lw $s1, 8($sp)
			lw $s2, 12($sp)
			lw $s3, 16($sp)
			lw $s4, 20($sp)
  			addi $sp, $sp, 24
			jr $ra

		more:
			li $v1, 1
			lw $ra, 0($sp)
			lw $s0, 4($sp)
			lw $s1, 8($sp)
			lw $s2, 12($sp)
			lw $s3, 16($sp)
			lw $s4, 20($sp)
  			addi $sp, $sp, 24
			jr $ra

		increment:
			addi $s0, $s0, 1 # increment byte counter
			addi $s1, $s1, 1
			j loopComp # jump to loopComp 

trim: # delete empty line
	addi $sp, $sp, -4 # make frame 
	sw $ra, 0($sp)

	move $t0, $a0  # load input into $t0
	li $t1, 10 # load "\n" into $t1 (10: ASCII value for \n)
	match:
		lb $t2, 0($t0) # load a byte from $t0 and store it to $t2
		slt $t3, $t2, $t1  # set $t3 to 1 if $t2 is less than $t1
		slt $t3, $t1, $t2 # set $t3 to 1 if $t1 is less than $t2
		beq $t3, 0, delete 
		addi $t0, $t0, 1 # points to the next byte in $t0
		j match

	delete:
		sb $zero, 0($t0)
		j fin

	fin:
		lw $ra, 0($sp)
  		addi $sp, $sp, 4
		jr $ra

exit:
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	jr $ra 

.data
name: .space 64
done: 
.asciiz "DONE\n"
nln: 
.asciiz "\n"
promptName: 
.asciiz "Pizza name:\n"
promptDiameter: 
.asciiz "Pizza diameter:\n"
promptCost:
.asciiz "Pizza cost:\n"