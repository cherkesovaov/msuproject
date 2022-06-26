//#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define p(i,j) idata[j*iw+i]

typedef struct vert{
    int leftvert;
    short isleft;

    int topvert;
    short istop;

    int rightvert;
    short isright;

    int botvert;
    short isbot;

}vert;

void dfs(unsigned char *odata,vert *image,short *vis,short *col,int j,int i){
    vis[i]=j+1;
    odata[4*i]=col[3*(j%16)];
    odata[4*i+1]=col[3*(j%16)+1];
    odata[4*i+2]=col[3*(j%16)+2];
    odata[4*i+3]=255;
    if(image[i].isleft){
        if(!vis[image[i].leftvert]){
            dfs(odata,image,vis,col,j,image[i].leftvert);
        }
    }
    if(image[i].istop){
        if(!vis[image[i].topvert]){
            dfs(odata,image,vis,col,j,image[i].topvert);
        }
    }
    if(image[i].isright){
        if(!vis[image[i].rightvert]){
            dfs(odata,image,vis,col,j,image[i].rightvert);
        }
    }
    if(image[i].isbot){
        if(!vis[image[i].botvert]){
            dfs(odata,image,vis,col,j,image[i].botvert);
        }
    }
    return;
}


int main() {
//char* inputPath = "home/cherkesova/work/hampster.png";
char* inputPath="C:/Users/Olga/Desktop/hampster/hampster.png";
int iw, ih, n;
unsigned char *idata = stbi_load(inputPath, &iw, &ih, &n, 0);
if(idata==NULL){
    printf("ERROR: can`t read file %s\n", inputPath);
    return -1;
}
//printf("%d %d %d\n",iw,ih,n);
//unsigned char *pixel=idata;
//set colors
/*char red =pixel[0];
char blue = pixel[1];
char green = pixel[2];
//going to next pixel
pixel=pixel+n;*/
unsigned char *odata=(unsigned char*)malloc(iw*ih*n);
if(odata==NULL){
    printf("ERROR: no memory");
    return -1;
}
//near pixel
//int d=3;
//int s,t;
int i,j;
//image_iw*i+j_numbers
// 1
//0*2
// 3


//making image gray
unsigned char* grayImage = (unsigned char*)malloc(iw*ih*sizeof(unsigned char));
unsigned char* sharpImage = (unsigned char*)malloc(iw*ih*sizeof(unsigned char));
unsigned char* SobelImage = (unsigned char*)malloc(iw*ih*sizeof(unsigned char));
unsigned char* GaussImage = (unsigned char*)malloc(iw*ih*sizeof(unsigned char));
if(grayImage==NULL){
    printf("Memory allocation error at color_to_gray(): %d, %d\n",iw,ih);
        return 1;
}
int size;
size=iw*ih*n;
j=0;
for(i=0;i<size;i=i+n){
    grayImage[j]=0.299*idata[i]+0.587*idata[i+1]+0.114*idata[i+2];
    j++;
}
long long unsigned int g_size;
g_size=j;

//sharpness
int corr;
corr=iw+1;
/*for(i=corr;i<g_size-corr;i++){
    sharpImage[i]=(-grayImage[i-iw-1]-grayImage[i-iw]-grayImage[i-iw+1]-grayImage[i-1]+9*grayImage[i]-grayImage[i+1]-grayImage[i+iw-1]-grayImage[i+iw]-grayImage[i+iw+1]);
}*/
for(i=0;i<g_size;i++){
    if(grayImage[i]<90){
        sharpImage[i]=0;
    }
    else if(grayImage[i]<170){
        sharpImage[i]=128;
    }
    else{
        sharpImage[i]=255;
    }
}

//Sobel
for(i=corr;i<g_size-corr;i++){
    SobelImage[i]=sqrt(pow((-sharpImage[i-iw-1]-2.6*sharpImage[i-iw]-sharpImage[i-iw+1]+sharpImage[i+iw-1]+2.6*sharpImage[i+iw]+sharpImage[i+iw+1]),2)+pow((sharpImage[i-iw-1]-sharpImage[i-iw+1]+2.6*sharpImage[i-1]-2.6*sharpImage[i+1]+sharpImage[i+iw-1]-sharpImage[i+iw+1]),2));
}

//Gauss
for(i=corr;i<g_size-corr;i++){
    GaussImage[i]=(SobelImage[i-iw-1]+4*SobelImage[i-iw]+SobelImage[i-iw+1]+4*SobelImage[i-1]+8*SobelImage[i]+4*SobelImage[i+1]+SobelImage[i+iw-1]+4*SobelImage[i+iw]+SobelImage[i+iw+1])/28;
}

//making graph
vert *image;
image=(vert*)calloc(sizeof(vert),g_size);

int bord;
bord=10;
for(i=corr;i<g_size-corr;i++){
    if((abs(GaussImage[i]-GaussImage[i-1])<bord)&&(i-1>corr)){
        image[i-corr].leftvert=i-1-corr;
        image[(i-corr)].isleft=1;
    }
    if((abs(GaussImage[i]-GaussImage[i-iw])<bord)&&(i-iw>corr)){
        image[(i-corr)].topvert=i-iw-corr;
        image[(i-corr)].istop=1;
    }
    if((abs(GaussImage[i]-GaussImage[i+1])<bord)&&(i+1<g_size-corr)){
        image[(i-corr)].rightvert=i+1-corr;
        image[(i-corr)].isright=1;
    }
    if((abs(GaussImage[i]-GaussImage[i+iw])<bord)&&(i+iw<g_size-corr)){
        image[(i-corr)].botvert=i+iw-corr;
        image[(i-corr)].isbot=1;
    }
}
//array for DFS
short *vis;
vis=(short*)calloc(sizeof(short),g_size-2*corr);
//printf("WEEEEE\n");
//hamster_colors
short *col;
col=(short*)calloc(sizeof(short),48);
col[0]=148;
col[1]=0;
col[2]=211;

col[3]=0;
col[4]=255;
col[5]=255;

col[6]=70;
col[7]=130;
col[8]=180;

col[9]=0;
col[10]=0;
col[11]=139;

col[12]=102;
col[13]=205;
col[14]=170;

col[15]=65;
col[16]=105;
col[17]=225;

col[18]=75;
col[19]=0;
col[20]=130;

col[21]=72;
col[22]=61;
col[23]=139;

col[24]=25;
col[25]=25;
col[26]=112;

col[27]=106;
col[28]=90;
col[29]=205;

col[30]=147;
col[31]=112;
col[32]=219;

col[33]=127;
col[34]=225;
col[35]=212;

col[36]=0;
col[37]=128;
col[38]=128;

col[39]=143;
col[40]=188;
col[41]=143;

col[42]=0;
col[43]=139;
col[44]=139;

col[45]=224;
col[46]=255;
col[247]=255;


/*printf("%d\n",iw*ih*n);
printf("%d\n",g_size*4);*/


//DFS
j=-1;
int ow,oh;
ow=iw;
oh=ih;
for(i=0;i<g_size-2*corr;i++){
    if(!vis[i]){
        dfs(odata,image,vis,col,++j,i);
        //printf("%d\n",j);
    }
}

//Gauss again
/*for(i=corr;i<g_size-corr;i++){
    odata[i]=(odata[i-iw-1]+2*odata[i-iw]+odata[i-iw+1]+2*odata[i-1]+4*odata[i]+2*odata[i+1]+odata[i+iw-1]+2*odata[i+iw]+odata[i+iw+1])/16;
}*/

//output

//char *outputPath="home/cherkesova/work/Pictures/output.png";
char *outputPath="C:/Users/Olga/Desktop/hampster/output.png";
stbi_write_png(outputPath,ow,oh,n,odata,0);

free(col);
free(vis);
free(grayImage);
free(sharpImage);
free(SobelImage);
free(GaussImage);
free(image);
stbi_image_free(idata);
stbi_image_free(odata);
return 0;
}
