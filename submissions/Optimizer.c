/*
 *********************************************
 *  314 Principles of Programming Languages  *
 *  Spring 2017                              *
 *  Author: Ulrich Kremer                    *
 *  Student Version                          *
 *********************************************
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "InstrUtils.h"
#include "Utils.h"

void SearchLoad(Instruction *ptr, int reg, int offset);

void SearchStore(Instruction *ptr, int reg, int offset);

void SearchLoad(Instruction *ptr, int reg, int offset){
	while(ptr->prev){
		switch(ptr->opcode){
			case STOREAI:
				if(ptr->field2 == reg && ptr->field3 == offset){
					ptr->critical = '1';
					return;
				}
				break;

			case LOADAI:
			case ADD:
			case MUL:
			case SUB:
			case DIV:
				if(ptr->field3 == reg){
					ptr->critical = '1';
					return;
				}
				break;
			
			case LOADI:
				if(ptr->field2 == reg){
					ptr->critical = '1';
					return;
				}
				break;
			
			default:
				break;
		}
		ptr = ptr->prev;
	}
}

void SearchStore(Instruction *ptr, int reg, int offset){

	while(ptr->prev){
		if(ptr->opcode == STOREAI){
			if(ptr->field2 == reg && ptr->field3 == offset){
				ptr->critical = '1';
				return;
			}
		}

		ptr = ptr->prev;
	}	
}

int main()
{
	Instruction *head;
	
	head = ReadInstructionList(stdin);
	if (!head) {
		WARNING("No instructions\n");
		exit(EXIT_FAILURE);
	}

	/* YOUR CODE GOES HERE */
	Instruction *ptr = head;
	while(ptr->next){
		ptr = ptr->next;
	}	
	
	while(ptr->opcode != OUTPUTAI && ptr->prev){
		ptr = ptr->prev;
	}
	
	if(!ptr->prev){
		ptr->critical = '1';
	}
	head->critical = '1';
	
	while(ptr->prev){
		
		int reg, offset;
		switch(ptr->opcode){
			
			case OUTPUTAI:
				reg = ptr->field1;
				offset = ptr->field2;
				ptr->critical = '1';
				SearchStore(ptr, reg, offset);
				break;
			
			case STOREAI:
				if(ptr->critical == '1'){
					reg = ptr->field1;
					SearchLoad(ptr, reg, 0);
				}
				break;

			case ADD:
			case MUL:
			case SUB:
			case DIV:
				if(ptr->critical == '1'){
					reg = ptr->field1;
					SearchLoad(ptr, reg, 0);
					reg = ptr->field2;
					SearchLoad(ptr, reg, 0);
				}
				break;

			case LOADAI:
				if(ptr->critical == '1'){
					reg = ptr->field1;
					offset = ptr->field2;
					SearchLoad(ptr, reg, offset);
				}
				break;

			default:
				break;
		}
		ptr = ptr->prev;	
	}
	

	ptr = head;
	
	while(ptr->next){

		if(ptr->critical != '1'){
			if(ptr != head && ptr->next){
				ptr->prev->next = ptr->next;
				ptr->next->prev = ptr->prev;
				free(ptr);
			}
			else if(ptr == head && head->next){
				head->next->prev = NULL;
				Instruction *temp = head;
				head = head->next;
				free(temp);
			}

			else if(!ptr->next && ptr->prev){
				ptr->prev->next = NULL;
				free(ptr);
			}
		}
		ptr = ptr->next;
	}

	if (head) 
		PrintInstructionList(stdout, head);
	
	return EXIT_SUCCESS;
}
