#ifndef ENIGMA_H
#define ENIGMA_H

#include<stdio.h>
#include<stdlib.h>
#include <string.h>

/*char rotors[5][27]={"EKMFLGDQVZNTOWYHXUSPAIBRCJ", "AJDKSIRUXBLHWTMCQGZNPYFVOE","BDFHJLCPRTXVZNYEIWGAKMUSQO", "ESOVPZJAYQUIRHXLNFTGKDCMWB","VZBRGITYUPSDNHLXAWMJQOFECK"};*/

struct rotor{
	char base_permutation[27];
	char current_permutation[27];
	char ring_setting;
	char notch;
	char ground_setting;
};

struct enigma{
	struct rotor rotor1;
	struct rotor rotor2;
	struct rotor rotor3;;
	char reflector[27];
	char plugboard_permutation[27];

};

char inverse_letter(struct rotor* r, char c);

void rotor_and_reflector_permutation(struct enigma *e, char *out);

struct rotor initialize_rotor(char *base_permutation,char ground_setting, char ring_setting,char notch);

struct enigma initialize_enigma(char rotors[][27],char *notches, int index1, int index2, int index3, char* reflector, char* plugboard_permutation,char ground1, char ground2, char ground3, char ring1, char ring2, char ring3);

void rotate_rotor(struct rotor* r);

char encode_letter(struct enigma *e, char c);

char* encode(struct enigma* e, char *message, int n);

void rotate_rotors(struct enigma *e);


#endif
