/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bambi.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Bambi character structure
enum
{
	Bambi_ArcMain_Idle0,
	Bambi_ArcMain_Idle1,
	Bambi_ArcMain_Idle2,
	Bambi_ArcMain_Idle3,
	Bambi_ArcMain_Idle4,
	Bambi_ArcMain_Idle5,
	Bambi_ArcMain_Idle6,
	Bambi_ArcMain_Left0,
	Bambi_ArcMain_Left1,
	Bambi_ArcMain_Left2,
	Bambi_ArcMain_Left3,
	Bambi_ArcMain_Down0,
	Bambi_ArcMain_Down1,
	Bambi_ArcMain_Down2,
	Bambi_ArcMain_Down3,
	Bambi_ArcMain_Up0,
	Bambi_ArcMain_Up1,
	Bambi_ArcMain_Up2,
	Bambi_ArcMain_Up3,
	Bambi_ArcMain_Right0,
	Bambi_ArcMain_Right1,
	Bambi_ArcMain_Right2,
	Bambi_ArcMain_Right3,
	
	Bambi_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Bambi_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Bambi;

//Bambi character definitions
static const CharFrame char_bambi_frame[] = {
	{Bambi_ArcMain_Idle0, {  0,   0, 256, 256}, { 125, 250}}, //0 idle 1
	{Bambi_ArcMain_Idle1, {  0,   0, 256, 256}, { 125, 250}}, //1 idle 2
	{Bambi_ArcMain_Idle2, {  0,   0, 256, 256}, { 125, 250}}, //2 idle 3
	{Bambi_ArcMain_Idle3, {  0,   0, 256, 256}, { 125, 250}}, //3 idle 4
	{Bambi_ArcMain_Idle4, {  0,   0, 256, 256}, { 125, 250}}, //4 idle 5
	{Bambi_ArcMain_Idle5, {  0,   0, 256, 256}, { 125, 250}}, //5 idle 6
	{Bambi_ArcMain_Idle6, {  0,   0, 256, 256}, { 125, 250}}, //6 idle 7

	{Bambi_ArcMain_Left0, {  0,   0, 256, 256}, { 125, 250}}, //7 left 1
	{Bambi_ArcMain_Left1, {  0,   0, 256, 256}, { 125, 250}}, //8 left 2
	{Bambi_ArcMain_Left2, {  0,   0, 256, 256}, { 125, 250}}, //9 left 3
	{Bambi_ArcMain_Left3, {  0,   0, 256, 256}, { 125, 250}}, //10 left 4
	
	{Bambi_ArcMain_Down0, {  0,   0, 256, 256}, { 125, 250}}, //11 down 1
	{Bambi_ArcMain_Down1, {  0,   0, 256, 256}, { 125, 250}}, //12 down 2
	{Bambi_ArcMain_Down2, {  0,   0, 256, 256}, { 125, 250}}, //13 down 3
	{Bambi_ArcMain_Down3, {  0,   0, 256, 256}, { 125, 250}}, //14 down 4

	{Bambi_ArcMain_Up0, {   0,   0,  256, 256}, { 125, 250}}, //15 up 1
	{Bambi_ArcMain_Up1, {   0,   0,  256, 256}, { 125, 250}}, //16 up 2
	{Bambi_ArcMain_Up2, {   0,   0,  256, 256}, { 125, 250}}, //17 up 3
	{Bambi_ArcMain_Up3, {   0,   0,  256, 256}, { 125, 250}}, //18 up 4

	{Bambi_ArcMain_Right0, {  0,   0, 256, 256}, { 125, 250}}, //19 right 1
	{Bambi_ArcMain_Right1, {  0,   0, 256, 256}, { 125, 250}}, //20 right 2
	{Bambi_ArcMain_Right2, {  0,   0, 256, 256}, { 125, 250}}, //21 right 3
	{Bambi_ArcMain_Right3, {  0,   0, 256, 256}, { 125, 250}}, //22 right 4
};

static const Animation char_bambi_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6,  6, ASCR_BACK, 1}},      	   	           	   //CharAnim_Idle
	{2, (const u8[]){ 7,  8,  9, 10,  9,  8, ASCR_BACK, 1}},       		                           //CharAnim_Left
	{2, (const u8[]){ 7,  8,  9, 10,  9,  8, ASCR_BACK, 1}},                   			   //CharAnim_LeftAlt
	{2, (const u8[]){11, 12, 13, 14, 13, 12, ASCR_BACK, 1}},  		                           //CharAnim_Down
	{2, (const u8[]){11, 12, 13, 14, 13, 12, ASCR_BACK, 1}},   	          	                   //CharAnim_DownAlt
	{2, (const u8[]){15, 16, 17, 18, 17, 16, ASCR_BACK, 1}},              			           //CharAnim_Up
	{2, (const u8[]){15, 16, 17, 18, 17, 16, ASCR_BACK, 1}},                                           //CharAnim_UpAlt
	{2, (const u8[]){19, 20, 21, 22, 21, 20, ASCR_BACK, 1}},  					   //CharAnim_Right
	{2, (const u8[]){19, 20, 21, 22, 21, 20, ASCR_BACK, 1}},                             	           //CharAnim_RightAlt
};

//Bambi character functions
void Char_Bambi_SetFrame(void *user, u8 frame)
{
	Char_Bambi *this = (Char_Bambi*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bambi_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Bambi_Tick(Character *character)
{
	Char_Bambi *this = (Char_Bambi*)character;
	
	//Perform idle dance
	Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Bambi_SetFrame);
	Character_Draw(character, &this->tex, &char_bambi_frame[this->frame]);
}

void Char_Bambi_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Bambi_Free(Character *character)
{
	Char_Bambi *this = (Char_Bambi*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Bambi_New(fixed_t x, fixed_t y)
{
	//Allocate bambi object
	Char_Bambi *this = Mem_Alloc(sizeof(Char_Bambi));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Bambi_New] Failed to allocate bambi object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Bambi_Tick;
	this->character.set_anim = Char_Bambi_SetAnim;
	this->character.free = Char_Bambi_Free;
	
	Animatable_Init(&this->character.animatable, char_bambi_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character stage information
	this->character.health_i = 3;
	
	this->character.focus_x = FIXED_DEC(25,1);
	this->character.focus_y = FIXED_DEC(-90,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\BAMBI.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //Bambi_ArcMain_Idle0
		"idle1.tim", //Bambi_ArcMain_Idle1
		"idle2.tim", //Bambi_ArcMain_Idle2
		"idle3.tim", //Bambi_ArcMain_Idle3
		"idle4.tim", //Bambi_ArcMain_Idle4
		"idle5.tim", //Bambi_ArcMain_Idle5
		"idle6.tim", //Bambi_ArcMain_Idle6
		"left0.tim", //Bambi_ArcMain_Left0
		"left1.tim", //Bambi_ArcMain_Left1
		"left2.tim", //Bambi_ArcMain_Left2
		"left3.tim", //Bambi_ArcMain_Left3
		"down0.tim", //Bambi_ArcMain_Down0
		"down1.tim", //Bambi_ArcMain_Down1
		"down2.tim", //Bambi_ArcMain_Down2
		"down3.tim", //Bambi_ArcMain_Down3
		"up0.tim",   //Bambi_ArcMain_Up0
		"up1.tim",   //Bambi_ArcMain_Up1
		"up2.tim",   //Bambi_ArcMain_Up2
		"up3.tim",   //Bambi_ArcMain_Up3
		"right0.tim", //Bambi_ArcMain_Right0
		"right1.tim", //Bambi_ArcMain_Right1
		"right2.tim", //Bambi_ArcMain_Right2
		"right3.tim", //Bambi_ArcMain_Right3
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
