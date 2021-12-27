/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bambib.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//BambiB character structure
enum
{
	BambiB_ArcMain_Idle0,
	BambiB_ArcMain_Idle1,
	BambiB_ArcMain_Idle2,
	BambiB_ArcMain_Idle3,
	BambiB_ArcMain_Left0,
	BambiB_ArcMain_Left1,
	BambiB_ArcMain_Down0,
	BambiB_ArcMain_Down1,
	BambiB_ArcMain_Up0,
	BambiB_ArcMain_Up1,
	BambiB_ArcMain_Right0,
	BambiB_ArcMain_Right1,
	
	BambiB_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BambiB_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_BambiB;

//BambiB character definitions
static const CharFrame char_bambib_frame[] = {
	{BambiB_ArcMain_Idle0, {  0,   0, 256, 256}, { 125, 250}}, //0 idle 1
	{BambiB_ArcMain_Idle1, {  0,   0, 256, 256}, { 125, 250}}, //1 idle 2
	{BambiB_ArcMain_Idle2, {  0,   0, 256, 256}, { 125, 250}}, //2 idle 3
	{BambiB_ArcMain_Idle3, {  0,   0, 256, 256}, { 125, 250}}, //3 idle 4

	{BambiB_ArcMain_Left0, {  0,   0, 256, 256}, { 125, 250}}, //4 left 1
	{BambiB_ArcMain_Left1, {  0,   0, 256, 256}, { 125, 250}}, //5 left 2

	{BambiB_ArcMain_Down0, {  0,   0, 256, 256}, { 125, 250}}, //6 down 1
	{BambiB_ArcMain_Down1, {  0,   0, 256, 256}, { 125, 250}}, //7 down 2

	{BambiB_ArcMain_Up0, {   0,   0,  256, 256}, { 125, 250}}, //8 up 1
	{BambiB_ArcMain_Up1, {   0,   0,  256, 256}, { 125, 250}}, //9 up 2

	{BambiB_ArcMain_Right0, {  0,   0, 256, 256}, { 125, 250}}, //10 right 1
	{BambiB_ArcMain_Right1, {  0,   0, 256, 256}, { 125, 250}}, //11 right 2 
};

static const Animation char_bambib_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  3,  3, ASCR_BACK, 1}},      	   	           //CharAnim_Idle
	{2, (const u8[]){ 4,  5, ASCR_BACK, 1}},       		                           //CharAnim_Left
	{2, (const u8[]){ 4,  5, ASCR_BACK, 1}},                   			   //CharAnim_LeftAlt
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},  		                           //CharAnim_Down
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},   	          	                   //CharAnim_DownAlt
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},              			           //CharAnim_Up
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},                                           //CharAnim_UpAlt
	{2, (const u8[]){10, 11, ASCR_BACK, 1}},  					   //CharAnim_Right
	{2, (const u8[]){10, 11, ASCR_BACK, 1}},                             	           //CharAnim_RightAlt
};

//BambiB character functions
void Char_BambiB_SetFrame(void *user, u8 frame)
{
	Char_BambiB *this = (Char_BambiB*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bambib_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BambiB_Tick(Character *character)
{
	Char_BambiB *this = (Char_BambiB*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_BambiB_SetFrame);
	Character_Draw(character, &this->tex, &char_bambib_frame[this->frame]);
}

void Char_BambiB_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BambiB_Free(Character *character)
{
	Char_BambiB *this = (Char_BambiB*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BambiB_New(fixed_t x, fixed_t y)
{
	//Allocate bambib object
	Char_BambiB *this = Mem_Alloc(sizeof(Char_BambiB));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BambiB_New] Failed to allocate bambib object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BambiB_Tick;
	this->character.set_anim = Char_BambiB_SetAnim;
	this->character.free = Char_BambiB_Free;
	
	Animatable_Init(&this->character.animatable, char_bambib_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 5;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BAMBIB.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //BambiB_ArcMain_Idle0
		"idle1.tim", //BambiB_ArcMain_Idle1
		"idle2.tim", //BambiB_ArcMain_Idle2
		"idle3.tim", //BambiB_ArcMain_Idle3
		"left0.tim", //BambiB_ArcMain_Left0
		"left1.tim", //BambiB_ArcMain_Left1
		"down0.tim", //BambiB_ArcMain_Down0
		"down1.tim", //BambiB_ArcMain_Down1
		"up0.tim",   //BambiB_ArcMain_Up0
		"up1.tim",   //BambiB_ArcMain_Up1
		"right0.tim", //BambiB_ArcMain_Right0
		"right1.tim", //BambiB_ArcMain_Right1
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
