/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bambid.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//BambiD character structure
enum
{
	BambiD_ArcMain_Idle0,
	BambiD_ArcMain_Idle1,
	BambiD_ArcMain_Idle2,
	BambiD_ArcMain_Idle3,
	BambiD_ArcMain_Left0,
	BambiD_ArcMain_Left1,
	BambiD_ArcMain_Left2,
	BambiD_ArcMain_Left3,
	BambiD_ArcMain_Down0,
	BambiD_ArcMain_Down1,
	BambiD_ArcMain_Down2,
	BambiD_ArcMain_Down3,
	BambiD_ArcMain_Up0,
	BambiD_ArcMain_Up1,
	BambiD_ArcMain_Up2,
	BambiD_ArcMain_Up3,
	BambiD_ArcMain_Right0,
	BambiD_ArcMain_Right1,
	BambiD_ArcMain_Right2,
	BambiD_ArcMain_Right3,
	
	BambiD_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BambiD_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_BambiD;

//BambiD character definitions
static const CharFrame char_bambid_frame[] = {
	{BambiD_ArcMain_Idle0, {  0,   0, 256, 256}, { 125, 250}}, //0 idle 1
	{BambiD_ArcMain_Idle1, {  0,   0, 256, 256}, { 125, 250}}, //1 idle 2
	{BambiD_ArcMain_Idle2, {  0,   0, 256, 256}, { 125, 250}}, //2 idle 3
	{BambiD_ArcMain_Idle3, {  0,   0, 256, 256}, { 125, 250}}, //3 idle 4

	{BambiD_ArcMain_Left0, {  0,   0, 256, 256}, { 125, 250}}, //4 left 1
	{BambiD_ArcMain_Left1, {  0,   0, 256, 256}, { 125, 250}}, //5 left 2
	{BambiD_ArcMain_Left2, {  0,   0, 256, 256}, { 125, 250}}, //6 left 3
	{BambiD_ArcMain_Left3, {  0,   0, 256, 256}, { 125, 250}}, //7 left 4

	{BambiD_ArcMain_Down0, {  0,   0, 256, 256}, { 125, 250}}, //8 down 1
	{BambiD_ArcMain_Down1, {  0,   0, 256, 256}, { 125, 250}}, //9 down 2
	{BambiD_ArcMain_Down2, {  0,   0, 256, 256}, { 125, 250}}, //10 down 3
	{BambiD_ArcMain_Down3, {  0,   0, 256, 256}, { 125, 250}}, //11 down 4

	{BambiD_ArcMain_Up0, {   0,   0,  256, 256}, { 125, 250}}, //12 up 1
	{BambiD_ArcMain_Up1, {   0,   0,  256, 256}, { 125, 250}}, //13 up 2
	{BambiD_ArcMain_Up2, {   0,   0,  256, 256}, { 125, 250}}, //14 up 3
	{BambiD_ArcMain_Up3, {   0,   0,  256, 256}, { 125, 250}}, //15 up 4

	{BambiD_ArcMain_Right0, {  0,   0, 256, 256}, { 125, 250}}, //16 right 1
	{BambiD_ArcMain_Right1, {  0,   0, 256, 256}, { 125, 250}}, //17 right 2 
	{BambiD_ArcMain_Right2, {  0,   0, 256, 256}, { 125, 250}}, //18 right 3
	{BambiD_ArcMain_Right3, {  0,   0, 256, 256}, { 125, 250}}, //19 right 4 
};

static const Animation char_bambid_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3, ASCR_REPEAT, 1}},      	   	           //CharAnim_Idle
	{2, (const u8[]){ 4,  5,  6,  7, ASCR_BACK, 1}},       		                   //CharAnim_Left
	{2, (const u8[]){ 4,  5,  6,  7, ASCR_BACK, 1}},                   	           //CharAnim_LeftAlt
	{2, (const u8[]){ 8,  9, 10, 11, ASCR_BACK, 1}},  		                   //CharAnim_Down
	{2, (const u8[]){ 8,  9, 10, 11, ASCR_BACK, 1}},   	          	           //CharAnim_DownAlt
	{2, (const u8[]){12, 13, 14, 15, ASCR_BACK, 1}},              			   //CharAnim_Up
	{2, (const u8[]){12, 13, 14, 15, ASCR_BACK, 1}},                                   //CharAnim_UpAlt
	{2, (const u8[]){16, 17, 18, 19, ASCR_BACK, 1}},  				   //CharAnim_Right
	{2, (const u8[]){16, 17, 18, 19, ASCR_BACK, 1}},                             	   //CharAnim_RightAlt
};

//BambiD character functions
void Char_BambiD_SetFrame(void *user, u8 frame)
{
	Char_BambiD *this = (Char_BambiD*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bambid_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BambiD_Tick(Character *character)
{
	Char_BambiD *this = (Char_BambiD*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_BambiD_SetFrame);
	Character_Draw(character, &this->tex, &char_bambid_frame[this->frame]);
}

void Char_BambiD_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BambiD_Free(Character *character)
{
	Char_BambiD *this = (Char_BambiD*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BambiD_New(fixed_t x, fixed_t y)
{
	//Allocate bambid object
	Char_BambiD *this = Mem_Alloc(sizeof(Char_BambiD));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BambiD_New] Failed to allocate bambid object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BambiD_Tick;
	this->character.set_anim = Char_BambiD_SetAnim;
	this->character.free = Char_BambiD_Free;
	
	Animatable_Init(&this->character.animatable, char_bambid_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 3;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BAMBID.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //BambiD_ArcMain_Idle0
		"idle1.tim", //BambiD_ArcMain_Idle1
		"idle2.tim", //BambiD_ArcMain_Idle2
		"idle3.tim", //BambiD_ArcMain_Idle3
		"left0.tim", //BambiD_ArcMain_Left0
		"left1.tim", //BambiD_ArcMain_Left1
		"left2.tim", //BambiD_ArcMain_Left2
		"left3.tim", //BambiD_ArcMain_Left3
		"down0.tim", //BambiD_ArcMain_Down0
		"down1.tim", //BambiD_ArcMain_Down1
		"down2.tim", //BambiD_ArcMain_Down2
		"down3.tim", //BambiD_ArcMain_Down3
		"up0.tim",   //BambiD_ArcMain_Up0
		"up1.tim",   //BambiD_ArcMain_Up1
		"up2.tim",   //BambiD_ArcMain_Up2
		"up3.tim",   //BambiD_ArcMain_Up3
		"right0.tim", //BambiD_ArcMain_Right0
		"right1.tim", //BambiD_ArcMain_Right1
		"right2.tim", //BambiD_ArcMain_Right2
		"right3.tim", //BambiD_ArcMain_Right3
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
