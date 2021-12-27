/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "unfair.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Unfair character structure
enum
{
	Unfair_ArcMain_Idle0,
	Unfair_ArcMain_Idle1,
	Unfair_ArcMain_Idle2,
	Unfair_ArcMain_Idle3,
	Unfair_ArcMain_Left0,
	Unfair_ArcMain_Left1,
	Unfair_ArcMain_Left2,
	Unfair_ArcMain_Down0,
	Unfair_ArcMain_Down1,
	Unfair_ArcMain_Down2,
	Unfair_ArcMain_Up0,
	Unfair_ArcMain_Up1,
	Unfair_ArcMain_Up2,
	Unfair_ArcMain_Right0,
	Unfair_ArcMain_Right1,
	Unfair_ArcMain_Right2,
	
	Unfair_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Unfair_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Unfair;

//Unfair character definitions
static const CharFrame char_unfair_frame[] = {
	{Unfair_ArcMain_Idle0, {  0,   0, 256, 256}, { 125, 250}}, //0 idle 1
	{Unfair_ArcMain_Idle1, {  0,   0, 256, 256}, { 125, 250}}, //1 idle 2
	{Unfair_ArcMain_Idle2, {  0,   0, 256, 256}, { 125, 250}}, //2 idle 3
	{Unfair_ArcMain_Idle3, {  0,   0, 256, 256}, { 125, 250}}, //3 idle 4

	{Unfair_ArcMain_Left0, {  0,   0, 256, 256}, { 125, 250}}, //4 left 1
	{Unfair_ArcMain_Left1, {  0,   0, 256, 256}, { 125, 250}}, //5 left 2
	{Unfair_ArcMain_Left2, {  0,   0, 256, 256}, { 125, 250}}, //6 left 3
	
	{Unfair_ArcMain_Down0, {  0,   0, 256, 256}, { 125, 250}}, //7 down 1
	{Unfair_ArcMain_Down1, {  0,   0, 256, 256}, { 125, 250}}, //8 down 2
	{Unfair_ArcMain_Down2, {  0,   0, 256, 256}, { 125, 250}}, //9 down 3

	{Unfair_ArcMain_Up0, {   0,   0,  256, 256}, { 125, 250}}, //10 up 1
	{Unfair_ArcMain_Up1, {   0,   0,  256, 256}, { 125, 250}}, //11 up 2
	{Unfair_ArcMain_Up2, {   0,   0,  256, 256}, { 125, 250}}, //12 up 3

	{Unfair_ArcMain_Right0, {  0,   0, 256, 256}, { 125, 250}}, //13 right 1
	{Unfair_ArcMain_Right1, {  0,   0, 256, 256}, { 125, 250}}, //14 right 2
	{Unfair_ArcMain_Right2, {  0,   0, 256, 256}, { 125, 250}}, //15 right 3
};

static const Animation char_unfair_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  3,  3, ASCR_BACK, 1}},      	   	           		   //CharAnim_Idle
	{2, (const u8[]){ 4,  5,  6, ASCR_BACK, 1}},       		                                   //CharAnim_Left
	{2, (const u8[]){ 4,  5,  6, ASCR_BACK, 1}},                 			   		   //CharAnim_LeftAlt
	{2, (const u8[]){ 7,  8,  9, ASCR_BACK, 1}},     		                           	   //CharAnim_Down
	{2, (const u8[]){ 7,  8,  9, ASCR_BACK, 1}},      	          	                   	   //CharAnim_DownAlt
	{2, (const u8[]){10, 11, 12, ASCR_BACK, 1}},                			           	   //CharAnim_Up
	{2, (const u8[]){10, 11, 12, ASCR_BACK, 1}},                                            	   //CharAnim_UpAlt
	{2, (const u8[]){13, 14, 15, ASCR_BACK, 1}},     					  	   //CharAnim_Right
	{2, (const u8[]){13, 14, 15, ASCR_BACK, 1}},                                	           	   //CharAnim_RightAlt
};

//Unfair character functions
void Char_Unfair_SetFrame(void *user, u8 frame)
{
	Char_Unfair *this = (Char_Unfair*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_unfair_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Unfair_Tick(Character *character)
{
	Char_Unfair *this = (Char_Unfair*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Unfair_SetFrame);
	Character_Draw(character, &this->tex, &char_unfair_frame[this->frame]);
}

void Char_Unfair_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Unfair_Free(Character *character)
{
	Char_Unfair *this = (Char_Unfair*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Unfair_New(fixed_t x, fixed_t y)
{
	//Allocate unfair object
	Char_Unfair *this = Mem_Alloc(sizeof(Char_Unfair));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Unfair_New] Failed to allocate unfair object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Unfair_Tick;
	this->character.set_anim = Char_Unfair_SetAnim;
	this->character.free = Char_Unfair_Free;
	
	Animatable_Init(&this->character.animatable, char_unfair_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 11;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\UNFAIR.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Unfair_ArcMain_Idle0
		"idle1.tim", //Unfair_ArcMain_Idle1
		"idle2.tim", //Unfair_ArcMain_Idle2
		"idle3.tim", //Unfair_ArcMain_Idle3
		"left0.tim", //Unfair_ArcMain_Left0
		"left1.tim", //Unfair_ArcMain_Left1
		"left2.tim", //Unfair_ArcMain_Left2
		"down0.tim", //Unfair_ArcMain_Down0
		"down1.tim", //Unfair_ArcMain_Down1
		"down2.tim", //Unfair_ArcMain_Down2
		"up0.tim",   //Unfair_ArcMain_Up0
		"up1.tim",   //Unfair_ArcMain_Up1
		"up2.tim",   //Unfair_ArcMain_Up2
		"right0.tim", //Unfair_ArcMain_Right0
		"right1.tim", //Unfair_ArcMain_Right1
		"right2.tim", //Unfair_ArcMain_Right2
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
