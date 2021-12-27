/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "split.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Split character structure
enum
{
	Split_ArcMain_IdleA0,
	Split_ArcMain_IdleA1,
	Split_ArcMain_IdleA2,
	Split_ArcMain_IdleA3,
	Split_ArcMain_IdleA4,
	Split_ArcMain_IdleB0,
	Split_ArcMain_IdleB1,
	Split_ArcMain_IdleB2,
	Split_ArcMain_IdleB3,
	Split_ArcMain_LeftA0,
	Split_ArcMain_LeftA1,
	Split_ArcMain_LeftA2,
	Split_ArcMain_LeftA3,
	Split_ArcMain_LeftB0,
	Split_ArcMain_LeftB1,
	Split_ArcMain_LeftB2,
	Split_ArcMain_DownA0,
	Split_ArcMain_DownA1,
	Split_ArcMain_DownA2,
	Split_ArcMain_DownA3,
	Split_ArcMain_DownB0,
	Split_ArcMain_DownB1,
	Split_ArcMain_DownB2,
	Split_ArcMain_UpA0,
	Split_ArcMain_UpA1,
	Split_ArcMain_UpA2,
	Split_ArcMain_UpA3,
	Split_ArcMain_UpB0,
	Split_ArcMain_UpB1,
	Split_ArcMain_UpB2,
	Split_ArcMain_RightA0,
	Split_ArcMain_RightA1,
	Split_ArcMain_RightA2,
	Split_ArcMain_RightA3,
	Split_ArcMain_RightB0,
	Split_ArcMain_RightB1,
	Split_ArcMain_RightB2,
	
	Split_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Split_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Split;

//Split character definitions
static const CharFrame char_split_frame[] = {
	{Split_ArcMain_IdleA0, {  0,   0, 256, 256}, { 125, 250}}, //0 idle a 1
	{Split_ArcMain_IdleA1, {  0,   0, 256, 256}, { 125, 250}}, //1 idle a 2
	{Split_ArcMain_IdleA2, {  0,   0, 256, 256}, { 125, 250}}, //2 idle a 3
	{Split_ArcMain_IdleA3, {  0,   0, 256, 256}, { 125, 250}}, //3 idle a 4
	{Split_ArcMain_IdleA4, {  0,   0, 256, 256}, { 125, 250}}, //4 idle a 5

	{Split_ArcMain_IdleB0, {  0,   0, 256, 256}, { 125, 250}}, //5 idle b 1
	{Split_ArcMain_IdleB1, {  0,   0, 256, 256}, { 125, 250}}, //6 idle b 2
	{Split_ArcMain_IdleB2, {  0,   0, 256, 256}, { 125, 250}}, //7 idle b 1
	{Split_ArcMain_IdleB3, {  0,   0, 256, 256}, { 125, 250}}, //8 idle b 2

	{Split_ArcMain_LeftA0, {  0,   0, 256, 256}, { 125, 250}}, //9 left a 1
	{Split_ArcMain_LeftA1, {  0,   0, 256, 256}, { 125, 250}}, //10 left a 2
	{Split_ArcMain_LeftA2, {  0,   0, 256, 256}, { 125, 250}}, //11 left a 3
	{Split_ArcMain_LeftA3, {  0,   0, 256, 256}, { 125, 250}}, //12 left a 4

	{Split_ArcMain_LeftB0, {  0,   0, 256, 256}, { 125, 250}}, //13 left b 1
	{Split_ArcMain_LeftB1, {  0,   0, 256, 256}, { 125, 250}}, //14 left b 2
	{Split_ArcMain_LeftB2, {  0,   0, 256, 256}, { 125, 250}}, //15 left b 3
	
	{Split_ArcMain_DownA0, {  0,   0, 256, 256}, { 125, 250}}, //16 down a 1
	{Split_ArcMain_DownA1, {  0,   0, 256, 256}, { 125, 250}}, //17 down a 2
	{Split_ArcMain_DownA2, {  0,   0, 256, 256}, { 125, 250}}, //18 down a 3
	{Split_ArcMain_DownA3, {  0,   0, 256, 256}, { 125, 250}}, //19 down a 4

	{Split_ArcMain_DownB0, {  0,   0, 256, 256}, { 125, 250}}, //20 down b 1
	{Split_ArcMain_DownB1, {  0,   0, 256, 256}, { 125, 250}}, //21 down b 2
	{Split_ArcMain_DownB2, {  0,   0, 256, 256}, { 125, 250}}, //22 down b 3

	{Split_ArcMain_UpA0, {   0,   0,  256, 256}, { 125, 250}}, //23 up a 1
	{Split_ArcMain_UpA1, {   0,   0,  256, 256}, { 125, 250}}, //24 up a 2
	{Split_ArcMain_UpA2, {   0,   0,  256, 256}, { 125, 250}}, //25 up a 3
	{Split_ArcMain_UpA3, {   0,   0,  256, 256}, { 125, 250}}, //26 up a 4

	{Split_ArcMain_UpB0, {   0,   0,  256, 256}, { 125, 250}}, //27 up b 1
	{Split_ArcMain_UpB1, {   0,   0,  256, 256}, { 125, 250}}, //28 up b 2
	{Split_ArcMain_UpB2, {   0,   0,  256, 256}, { 125, 250}}, //29 up b 3

	{Split_ArcMain_RightA0, {  0,   0, 256, 256}, { 125, 250}}, //30 right a 1
	{Split_ArcMain_RightA1, {  0,   0, 256, 256}, { 125, 250}}, //31 right a 2
	{Split_ArcMain_RightA2, {  0,   0, 256, 256}, { 125, 250}}, //32 right a 3
	{Split_ArcMain_RightA3, {  0,   0, 256, 256}, { 125, 250}}, //33 right a 4

	{Split_ArcMain_RightB0, {  0,   0, 256, 256}, { 125, 250}}, //34 right b 1
	{Split_ArcMain_RightB1, {  0,   0, 256, 256}, { 125, 250}}, //35 right b 2
	{Split_ArcMain_RightB2, {  0,   0, 256, 256}, { 125, 250}}, //36 right b 3
};

static const Animation char_split_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  4, ASCR_BACK, 1}},      	   	           	   	   //CharAnim_Idle
	{2, (const u8[]){ 9, 10, 11, 12, ASCR_BACK, 1}},       		                           	   //CharAnim_Left
	{2, (const u8[]){13, 14, 15, 15, ASCR_CHGANI, CharAnim_IdleAlt}},                   		   //CharAnim_LeftAlt
	{2, (const u8[]){16, 17, 18, 19, ASCR_BACK, 1}},  		                           	   //CharAnim_Down
	{2, (const u8[]){20, 21, 22, 22, ASCR_CHGANI, CharAnim_IdleAlt}},   	          	           //CharAnim_DownAlt
	{2, (const u8[]){23, 24, 25, 26, ASCR_BACK, 1}},              			           	   //CharAnim_Up
	{2, (const u8[]){27, 28, 29, 29, ASCR_CHGANI, CharAnim_IdleAlt}},                                  //CharAnim_UpAlt
	{2, (const u8[]){30, 31, 32, 33, ASCR_BACK, 1}},  					   	   //CharAnim_Right
	{2, (const u8[]){34, 35, 36, 36, ASCR_CHGANI, CharAnim_IdleAlt}},                             	   //CharAnim_RightAlt
	{2, (const u8[]){ 5,  6,  7,  8,  8,  8, ASCR_CHGANI, CharAnim_IdleAlt}},      	   	           //CharAnim_IdleAlt
};

//Split character functions
void Char_Split_SetFrame(void *user, u8 frame)
{
	Char_Split *this = (Char_Split*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_split_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Split_Tick(Character *character)
{
	Char_Split *this = (Char_Split*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Split_SetFrame);
	Character_Draw(character, &this->tex, &char_split_frame[this->frame]);
}

void Char_Split_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Split_Free(Character *character)
{
	Char_Split *this = (Char_Split*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Split_New(fixed_t x, fixed_t y)
{
	//Allocate split object
	Char_Split *this = Mem_Alloc(sizeof(Char_Split));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Split_New] Failed to allocate split object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Split_Tick;
	this->character.set_anim = Char_Split_SetAnim;
	this->character.free = Char_Split_Free;
	
	Animatable_Init(&this->character.animatable, char_split_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 4;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\SPLIT.ARC;1");
	
	const char **pathp = (const char *[]){
		"idlea0.tim", //Split_ArcMain_IdleA0
		"idlea1.tim", //Split_ArcMain_IdleA1
		"idlea2.tim", //Split_ArcMain_IdleA2
		"idlea3.tim", //Split_ArcMain_IdleA3
		"idlea4.tim", //Split_ArcMain_IdleA4
		"idleb0.tim", //Split_ArcMain_IdleB0
		"idleb1.tim", //Split_ArcMain_IdleB1
		"idleb2.tim", //Split_ArcMain_IdleB2
		"idleb3.tim", //Split_ArcMain_IdleB3
		"lefta0.tim", //Split_ArcMain_LeftA0
		"lefta1.tim", //Split_ArcMain_LeftA1
		"lefta2.tim", //Split_ArcMain_LeftA2
		"lefta3.tim", //Split_ArcMain_LeftA3
		"leftb0.tim", //Split_ArcMain_LeftB0
		"leftb1.tim", //Split_ArcMain_LeftB1
		"leftb2.tim", //Split_ArcMain_LeftB2
		"downa0.tim", //Split_ArcMain_DownA0
		"downa1.tim", //Split_ArcMain_DownA1
		"downa2.tim", //Split_ArcMain_DownA2
		"downa3.tim", //Split_ArcMain_DownA3
		"downb0.tim", //Split_ArcMain_DownB0
		"downb1.tim", //Split_ArcMain_DownB1
		"downb2.tim", //Split_ArcMain_DownB2
		"upa0.tim",   //Split_ArcMain_UpA0
		"upa1.tim",   //Split_ArcMain_UpA1
		"upa2.tim",   //Split_ArcMain_UpA2
		"upa3.tim",   //Split_ArcMain_UpA3
		"upb0.tim",   //Split_ArcMain_UpB0
		"upb1.tim",   //Split_ArcMain_UpB1
		"upb2.tim",   //Split_ArcMain_UpB2
		"righta0.tim", //Split_ArcMain_RightA0
		"righta1.tim", //Split_ArcMain_RightA1
		"righta2.tim", //Split_ArcMain_RightA2
		"righta3.tim", //Split_ArcMain_RightA3
		"rightb0.tim", //Split_ArcMain_RightB0
		"rightb1.tim", //Split_ArcMain_RightB1
		"rightb2.tim", //Split_ArcMain_RightB2
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
