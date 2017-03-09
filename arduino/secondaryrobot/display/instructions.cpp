#include <Arduino.h>
#include "instructions.h"
#include "IPDisplay.h"
#include <EEPROM.h>

// Global variables
extern Ipdisplay ipdisplay;

// Instructions
void SET_MATRIX_MESSAGE(SerialTalks &talks, Deserializer &input, Serializer &output)
{
	// parametres a recuperer : 
	// numero de la matrice
	// type d'affichage (continu, défilement)
	// chaine de caractère a afficher
}

void SET_IPDISPLAY_MESSAGE(SerialTalks &talks, Deserializer &input, Serializer &output)
{
	
    char buffer[IP_DISPLAY_BUFFER_SIZE] = "";
    input >> buffer;
	output << buffer;
    //TODO : gerer le cas du depacement de la taille du buffer

    int nbDigits = 0;
    int shift = 0;
    for (int i = 0; i < IP_DISPLAY_BUFFER_SIZE && buffer[i]!='\0'; i++)
    {
        if (buffer[i] >= START_CHAR && buffer[i] <= END_CHAR)
        {
            nbDigits++;
			if(buffer[i+1] == '.' && buffer[i] != '.'){	// gère le cas de l'affichage d'un point après un digit
				i++;
			}
        }
    }
	output << nbDigits;
    shift = (12 - nbDigits) / 2;
	output << shift;
    ipdisplay.computeBuffer(buffer, shift);
}


#ifdef INIT_EEPROM
	void INIT_EEPROM_DATA(SerialTalks &talks, Deserializer &input, Serializer &output)
	{
		for(int i = 0;i<EEPROM_IPDISPLAY_LENGTH;i++){
			EEPROM.write(EEPROM_IPDISPLAY_START_ADDRESS+i,segToDisplay[i]);	//segToDisplay[i]
		}
	}
#endif