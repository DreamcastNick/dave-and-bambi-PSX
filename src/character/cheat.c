/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "cheat.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Cheat character structure
enum
{
	Cheat_ArcMain_Idle0,
	Cheat_ArcMain_Idle1,
	Cheat_ArcMain_Idle2,
	Cheat_ArcMain_Left0,
	Cheat_ArcMain_Left1,
	Cheat_ArcMain_Down0,
	Cheat_ArcMain_Down1,
	Cheat_ArcMain_Up0,
	Cheat_ArcMain_Up1,
	Cheat_ArcMain_Right0,
	Cheat_ArcMain_Right1,
	
	Cheat_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Cheat_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Cheat;

//Cheat character definitions
static const CharFrame char_cheat_frame[] = {
	{Cheat_ArcMain_Idle0, {  0,   0, 256, 256}, { 125, 250}}, //0 idle 1
	{Cheat_ArcMain_Idle1, {  0,   0, 256, 256}, { 125, 250}}, //1 idle 2
	{Cheat_ArcMain_Idle2, {  0,   0, 256, 256}, { 125, 250}}, //2 idle 3

	{Cheat_ArcMain_Left0, {  0,   0, 256, 256}, { 125, 250}}, //3 left 1
	{Cheat_ArcMain_Left1, {  0,   0, 256, 256}, { 125, 250}}, //4 left 2

	{Cheat_ArcMain_Down0, {  0,   0, 256, 256}, { 125, 250}}, //5 down 1
	{Cheat_ArcMain_Down1, {  0,   0, 256, 256}, { 125, 250}}, //6 down 2

	{Cheat_ArcMain_Up0, {   0,   0,  256, 256}, { 125, 250}}, //7 up 1
	{Cheat_ArcMain_Up1, {   0,   0,  256, 256}, { 125, 250}}, //8 up 2

	{Cheat_ArcMain_Right0, {  0,   0, 256, 256}, { 125, 250}}, //9 right 1
	{Cheat_ArcMain_Right1, {  0,   0, 256, 256}, { 125, 250}}, //10 right 2 
};

static const Animation char_cheat_anim[CharAnim_Max] = {
	{1, (const u8[]){ 0,  1,  2, ASCR_BACK, 1}},      	   	                   //CharAnim_Idle
	{2, (const u8[]){ 3,  4, ASCR_BACK, 1}},       		                           //CharAnim_Left
	{2, (const u8[]){ 3,  4, ASCR_BACK, 1}},                   			   //CharAnim_LeftAlt
	{2, (const u8[]){ 5,  6, ASCR_BACK, 1}},  		                           //CharAnim_Down
	{2, (const u8[]){ 5,  6, ASCR_BACK, 1}},   	          	                   //CharAnim_DownAlt
	{2, (const u8[]){ 7,  8, ASCR_BACK, 1}},              			           //CharAnim_Up
	{2, (const u8[]){ 7,  8, ASCR_BACK, 1}},                                           //CharAnim_UpAlt
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},  					   //CharAnim_Right
	{2, (const u8[]){ 9, 10, ASCR_BACK, 1}},                             	           //CharAnim_RightAlt
};

//Cheat character functions
void Char_Cheat_SetFrame(void *user, u8 frame)
{
	Char_Cheat *this = (Char_Cheat*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_cheat_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Cheat_Tick(Character *character)
{
	Char_Cheat *this = (Char_Cheat*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Cheat_SetFrame);
	Character_Draw(character, &this->tex, &char_cheat_frame[this->frame]);
}

void Char_Cheat_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Cheat_Free(Character *character)
{
	Char_Cheat *this = (Char_Cheat*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Cheat_New(fixed_t x, fixed_t y)
{
	//Allocate cheat object
	Char_Cheat *this = Mem_Alloc(sizeof(Char_Cheat));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Cheat_New] Failed to allocate cheat object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Cheat_Tick;
	this->character.set_anim = Char_Cheat_SetAnim;
	this->character.free = Char_Cheat_Free;
	
	Animatable_Init(&this->character.animatable, char_cheat_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 6;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\CHEAT.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Cheat_ArcMain_Idle0
		"idle1.tim", //Cheat_ArcMain_Idle1
		"idle2.tim", //Cheat_ArcMain_Idle2
		"left0.tim", //Cheat_ArcMain_Left0
		"left1.tim", //Cheat_ArcMain_Left1
		"down0.tim", //Cheat_ArcMain_Down0
		"down1.tim", //Cheat_ArcMain_Down1
		"up0.tim",   //Cheat_ArcMain_Up0
		"up1.tim",   //Cheat_ArcMain_Up1
		"right0.tim", //Cheat_ArcMain_Right0
		"right1.tim", //Cheat_ArcMain_Right1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
