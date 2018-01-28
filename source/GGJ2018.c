/*---------------------------------------------------------------------------------

	nehe lesson 2 used as base

---------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>

 
#define DEFAULT_FIFO_SIZE	(256*1024)

#define MAX_PLAYER_COUNT 4
#define MAX_BULLET_COUNT 256

typedef struct Player{
    s32 dead;
    s32 cooldown;
    f32 rotation;
    f32 scale;
    f32 xPos;
    f32 yPos;
    u8 playerNumber;
    u8 active;    
}Player;

typedef struct Bullet{
    f32 xVelocity;
    f32 yVelocity;
    f32 xPos;
    f32 yPos;
    u8 ownerNumber;
    u8 active;
}Bullet;

static u32 fb = 0; 	// initial framebuffer index
static Player players[MAX_PLAYER_COUNT] = {0};
static Bullet bullets[MAX_BULLET_COUNT] = {0};
 
static void *frameBuffer[2] = { NULL, NULL};
GXRModeObj *rmode;

static void drawPlayers(Mtx view){

    for(s32 i = 0; i < MAX_PLAYER_COUNT; i++){
        if(players[i].active && !players[i].dead){
            Mtx model, modelview;
            guMtxIdentity(model);
            guMtxTransApply(model, model, 0.0f,0.0f,-6.0f);
            guMtxConcat(view,model,modelview);
            // load the modelview matrix into matrix memory
            GX_LoadPosMtxImm(modelview, GX_PNMTX0);
            GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3);
            
            f32 scale = players[i].scale;
            f32 xPos = players[i].xPos;
            f32 yPos = players[i].yPos;
            
            f32 angle = players[i].rotation;
            
            f32 x1 = ( 0.0f*scale) * cosf(angle) - ( 0.25f*scale) * sinf(angle);
            f32 x2 = (-0.25f*scale) * cosf(angle) - (-0.25f*scale) * sinf(angle);
            f32 x3 = ( 0.25f*scale) * cosf(angle) - (-0.25f*scale) * sinf(angle);
            
            f32 y1 = ( 0.25f*scale) * cosf(angle) + ( 0.0f*scale) * sinf(angle);
            f32 y2 = (-0.25f*scale) * cosf(angle) + (-0.25f*scale) * sinf(angle);
            f32 y3 = (-0.25f*scale) * cosf(angle) + ( 0.25f*scale) * sinf(angle);
            
            GX_Position3f32(x1 + xPos, y1 + yPos, 0.0f);	// Top
			GX_Position3f32(x2 + xPos, y2 + yPos, 0.0f);	// Bottom Left
			GX_Position3f32(x3 + xPos, y3 + yPos, 0.0f);	// Bottom Right
            
            GX_End();
        }
    }
    
    
}

static void drawBullets(Mtx view){
    f32 pi = 3.1415926f;
    f32 piUnder180 = (180.0f / pi);
    for(s32 i = 0; i < MAX_BULLET_COUNT; i++){
        if(bullets[i].active){
            Mtx model, modelview;
            guMtxIdentity(model);
            guMtxTransApply(model, model, 0.0f,0.0f,-6.0f);
            guMtxConcat(view,model,modelview);
            // load the modelview matrix into matrix memory
            GX_LoadPosMtxImm(modelview, GX_PNMTX0);
            GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3);
            
            f32 scale = players[bullets[i].ownerNumber].scale;
            f32 xPos = bullets[i].xPos;
            f32 yPos = bullets[i].yPos;
            
            

            f32 angle = atan2(bullets[i].yVelocity, bullets[i].xVelocity);
            angle = angle * piUnder180;
            
            f32 x1 = ( 0.0f*scale) * cosf(angle) - ( 0.1f*scale) * sinf(angle);
            f32 x2 = (-0.1f*scale) * cosf(angle) - (-0.1f*scale) * sinf(angle);
            f32 x3 = ( 0.1f*scale) * cosf(angle) - (-0.1f*scale) * sinf(angle);
            
            f32 y1 = ( 0.1f*scale) * cosf(angle) + ( 0.0f*scale) * sinf(angle);
            f32 y2 = (-0.1f*scale) * cosf(angle) + (-0.1f*scale) * sinf(angle);
            f32 y3 = (-0.1f*scale) * cosf(angle) + ( 0.1f*scale) * sinf(angle);
            
            GX_Position3f32(x1  + xPos, y1 + yPos, 0.0f);	// Top
			GX_Position3f32(x2 + xPos, y2 + yPos, 0.0f);	// Bottom Left
			GX_Position3f32( x3 + xPos, y3 + yPos, 0.0f);	// Bottom Right
            
            GX_End();
        }
    }
    
}

static void draw(Mtx view){
    // do this before drawing
    GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
    drawPlayers(view);
    drawBullets(view);
    GX_DrawDone();
    
    fb ^= 1;		// flip framebuffer
    GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GX_SetColorUpdate(GX_TRUE);
    GX_CopyDisp(frameBuffer[fb],GX_TRUE);

    VIDEO_SetNextFramebuffer(frameBuffer[fb]);

    VIDEO_Flush();

    VIDEO_WaitVSync();
}

static s32 inititializePlayers(){
    players[0].dead = 0;
    players[0].cooldown = 0;
    players[0].scale = 2.0f;
    players[0].xPos = -1.75f;
    players[0].yPos = 1.75f;
    players[0].playerNumber = 0;
    players[0].active = 0;
    
    players[1].dead = 0;
    players[1].cooldown = 0;
    players[1].scale = 2.0f;
    players[1].xPos = 1.75f;
    players[1].yPos = -1.75f;
    players[1].playerNumber = 1;
    players[1].active = 0;
    
    players[2].dead = 0;
    players[2].cooldown = 0;
    players[2].scale = 2.0f;
    players[2].xPos = -1.75f;
    players[2].yPos = 1.75f;
    players[2].playerNumber = 2;
    players[2].active = 0;
    
    players[3].dead = 0;
    players[3].cooldown = 0;
    players[3].scale = 2.0f;
    players[3].xPos = 1.75f;
    players[3].yPos = 1.75f;
    players[3].playerNumber = 3;
    players[3].active = 0;
    
    return 0;
}

static void handlePlayer(PADStatus *padStatus, u8 playerNumber){
    
    // Activate players based on plugged in controllers
    if(padStatus[playerNumber].err != PAD_ERR_NONE){
        players[playerNumber].active = 0;
        return;
    }
    else{
        players[playerNumber].active = 1;
    }
    
    // If player presses start then quit
    if ( padStatus[playerNumber].button & PAD_BUTTON_START) {
        exit(0);
    }
    
    if(players[playerNumber].dead){
        return;
    }
    
    // Drop cooldown
    if(players[playerNumber].cooldown != 0){
        players[playerNumber].cooldown--;
    }
    
    f32 pi = 3.1415926f;
    f32 piUnder180 = (180.0f / pi);
    
    // Handle movement
    f32 movementMultiplier = 0.0005f;
    
    // Normalize sticks around zero
    s8 stickY = padStatus[playerNumber].stickY;
    s8 stickX = padStatus[playerNumber].stickX;
    
    players[playerNumber].xPos += stickX * movementMultiplier;
    players[playerNumber].yPos += stickY * movementMultiplier;
    
    // Handle rotation
    f32 angle;
    if(stickY == 0 && stickX == 0){
        angle = 0;
    }
    else if(stickY == 0){
        if(stickX > 0){
            angle = 90.0f;
        }
        else{
            angle = -90.0f;
        }
    }
    else if(stickX == 0){
        if(stickY > 0){
            angle = 0.0f;
        }
        else{
            angle = 180.0f;
        }
    }
    else{
        angle = atan2((f32)stickY, (f32)stickX);
        angle -= 90;
        if(angle < 0){
            angle += 360.0f;
        }
    }
    angle = angle * piUnder180;
    players[playerNumber].rotation = angle;

    
    // Handle Shooting
    if(padStatus[playerNumber].button & PAD_BUTTON_A && !players[playerNumber].cooldown){
        // Find an available bullet slot through a fancy algorithm
        s32 bulletIndex = -1;
        for(s32 i = 0; i < MAX_BULLET_COUNT; i++){
            if(bullets[i].active == 0){
                bulletIndex = i;
                break;
            }
        }
        
        if(bulletIndex == -1){
            // No free bullet :(
            goto endShootingHandling;
        }

        f32 bulletMultiplier = 0.0015;
    
        bullets[bulletIndex].xVelocity = stickX * bulletMultiplier;
        bullets[bulletIndex].yVelocity = stickY * bulletMultiplier;
        
        bullets[bulletIndex].xPos = players[playerNumber].xPos;
        bullets[bulletIndex].yPos = players[playerNumber].yPos;
        
        bullets[bulletIndex].ownerNumber = playerNumber;
        
        bullets[bulletIndex].active = 1;
        
        players[playerNumber].cooldown = 20;
    }
    endShootingHandling:
    return;
}

static void handleBullets(){
    for(s32 i = 0; i < MAX_BULLET_COUNT; i++){
        if(bullets[i].active){
            bullets[i].xPos += bullets[i].xVelocity;
            bullets[i].yPos += bullets[i].yVelocity;
            
            if(bullets[i].xPos < -3.0f || bullets[i].xPos > 3.0f ||
                 bullets[i].yPos < -3.0f ||  bullets[i].yPos > 3.0f){
                bullets[i].active = 0;
                continue;
            }
            
            // Check for collision with a player other than our owner
            for(s32 j = 0; j < MAX_PLAYER_COUNT; j++){
                if(j != bullets[i].ownerNumber){
                    // Simple circle distance check
                    f32 radius1 = players[bullets[i].ownerNumber].scale * 0.1f;
                    f32 radius2 = players[j].scale * 0.25f;
                    f32 totalRadiusSquared = radius1 + radius2;
                    totalRadiusSquared *= totalRadiusSquared;
                    
                    f32 xDist = (bullets[i].xPos - players[j].xPos);
                    f32 yDist = (bullets[i].yPos - players[j].yPos);
                    f32 distSquared = (xDist * xDist) + (yDist * yDist);
                    
                    // If hit
                    if(distSquared < totalRadiusSquared){
                        bullets[i].active = 0;
                        players[j].scale *= 0.85f;
                        if(players[j].scale < 0.5f){
                            players[j].dead = 1;
                        }
                        players[bullets[i].ownerNumber].scale *= 1.15f;
                        if(players[bullets[i].ownerNumber].scale > 5.0f){
                            players[bullets[i].ownerNumber].scale = 5.0f;
                        }
                        break;

                    }
                }
            }
        }
    }
}

//---------------------------------------------------------------------------------
int main( int argc, char **argv ){
//---------------------------------------------------------------------------------
	f32 yscale;

	u32 xfbHeight;

	Mtx view;
	Mtx44 perspective;
	

	GXColor background = {0, 0, 0, 0xff};


	// init the vi.
	VIDEO_Init();
 
	rmode = VIDEO_GetPreferredMode(NULL);
	PAD_Init();
	
	// allocate 2 framebuffers for double buffering
	frameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	frameBuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(frameBuffer[fb]);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	// setup the fifo and then init the flipper
	void *gp_fifo = NULL;
	gp_fifo = memalign(32,DEFAULT_FIFO_SIZE);
	memset(gp_fifo,0,DEFAULT_FIFO_SIZE);
 
	GX_Init(gp_fifo,DEFAULT_FIFO_SIZE);
 
	// clears the bg to color and clears the z buffer
	GX_SetCopyClear(background, 0x00ffffff);
 
	// other gx setup
	GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
	yscale = GX_GetYScaleFactor(rmode->efbHeight,rmode->xfbHeight);
	xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopySrc(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopyDst(rmode->fbWidth,xfbHeight);
	GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,GX_TRUE,rmode->vfilter);
	GX_SetFieldMode(rmode->field_rendering,((rmode->viHeight==2*rmode->xfbHeight)?GX_ENABLE:GX_DISABLE));
 
	GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(frameBuffer[fb],GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);
 

	// setup the vertex descriptor
	// tells the flipper to expect direct data
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
 
	// setup the vertex attribute table
	// describes the data
	// args: vat location 0-7, type of data, data format, size, scale
	// so for ex. in the first call we are sending position data with
	// 3 values X,Y,Z of size F32. scale sets the number of fractional
	// bits for non float data.
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
 
	GX_SetNumChans(1);
	GX_SetNumTexGens(0);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

	// setup our camera at the origin
	// looking down the -z axis with y up
	guVector cam = {0.0F, 0.0F, 0.0F},
			up = {0.0F, 1.0F, 0.0F},
		  look = {0.0F, 0.0F, -1.0F};
	guLookAt(view, &cam, &up, &look);
 

	// setup our projection matrix
	// this creates a perspective matrix with a view angle of 90,
	// and aspect ratio based on the display resolution
    f32 w = rmode->viWidth;
    f32 h = rmode->viHeight;
	guPerspective(perspective, 45, (f32)w/h, 0.1F, 300.0F);
	GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);
    
    // Initialize Players
    inititializePlayers();
    
    PADStatus padStatus[4] = {0};
    
    u32 padSyncCounter = 0;
    
    u32 resetCounter = 0;
    u32 gameOver = 0;
    
	while(1) {
        padSyncCounter++;
        
        if(padSyncCounter == 120){
            padSyncCounter = 0;
            u32 syncBits = 0;
            if(!players[0].active){
                syncBits |= PAD_CHAN0_BIT;
            }
            if(!players[1].active){
                syncBits |= PAD_CHAN1_BIT;
            }
            if(!players[2].active){
                syncBits |= PAD_CHAN2_BIT;
            }
            if(!players[3].active){
                syncBits |= PAD_CHAN3_BIT;
            }
            if(syncBits){
                PAD_Reset(syncBits);
                PAD_Sync();
            }
        }
    
        PAD_Read(padStatus);
    
        for(s32 i = 0; i < MAX_PLAYER_COUNT; i++){
            handlePlayer(padStatus, i);
        }
        
        handleBullets();
        
        if(!gameOver){
            s32 aliveCount = 0;
            s32 playerCount = 0;
            
            for(s32 i = 0; i < MAX_PLAYER_COUNT; i++){
                if(players[i].active){
                    if(players[i].dead == 0){
                        aliveCount++;
                    }
                    playerCount++;
                }
            }
            
            if(aliveCount <= 1 && playerCount > 1){
                gameOver = 1;
                resetCounter = 150;
            }
        }
        
        if(gameOver){
            if(resetCounter > 0){
                resetCounter--;
                for(s32 i = 0; i < MAX_PLAYER_COUNT; i++){
                    if(players[i].active && !players[i].dead){
                        players[i].scale *= 1.1f;
                    }
                }
            }
            else{
                gameOver = 0;
                inititializePlayers();
            }
        }
		
        draw(view);

	}
	return 0;
}
 
