#include <glut/glut.h> //MAC の人は GL を glut に変更
#include <iostream>
#include <algorithm>
#include <vector>
#include <deque>
#include <queue>
#include <list>
#include <stack>
#include <string>
#include <functional>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <map>
#include <set>
#include <cstdlib>
#include <stdlib.h>
#include <time.h>
#define INF 100000000
#define INFLL (long long)1e14
#define Loops(i, n) for(int i = 0; i < (int)n; i++)
#define Loops1(i, n) for(int i = 1; i <= (int)n; i++)
//パラメータ
#define PARENTS 640//親の数
#define ELITE 20//エリートは次世代へ
#define PROB_MUTATION 0.07//突然変異率
#define MULT_UNIF 0.1//一様交叉の確率
typedef std::pair<double,double> P;
void init(void);
void init_greedy();//貪欲法を混ぜた初期化
void Calc();//適応度計算
void Mult();//交叉
void Mutation();//突然変異
void mult_two_points(int x, int y);//二点交叉
void mult_one_point(int x,int y);//一点交叉
void mult_unif(int x,int y);//一様交叉
//汎用関数
static void DrawString(std::string str, int w, int h, int x0, int y0);//文字描画
int Prob(double x);
double distance(P p1,P p2);//都市間の距離計算
std::vector<int> G[PARENTS];//遺伝子
std::vector<double> Fitness(PARENTS);//適応度
std::vector<P> city;//都市データ
std::vector<int> Gtmp1,Gtmp2;//交叉用
int N;//都市数
int generation=0;//世代
int width, height;              /* ウィンドウの大きさ */
//ディスプレイ表示用
double Mincost_prev=INF;
double Mincost_prev2=INF;
double Mincost_first=-2;
int timer=0;

void display(void)
{        
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);
    Loops(i,N){//都市の描画
        glColor3d(1.0, 1.0, 1.0);
        glBegin(GL_POLYGON);
        glVertex2d( city[i].first*2-3, city[i].second*2-3 );
        glVertex2d( city[i].first*2+3, city[i].second*2-3 );
        glVertex2d( city[i].first*2+3, city[i].second*2+3 );
        glVertex2d( city[i].first*2-3, city[i].second*2+3 );
        glEnd();  
    }
    //巡回ルートの描画
    glBegin( GL_LINE_LOOP );
    std::vector<int>tmp;
    Loops(k,N){
            tmp.push_back(k);
    }
    int nowcity=0;
    Loops(i,N){
        int nowcity=tmp[G[0][i]];
        tmp.erase(tmp.begin()+G[0][i]);
        glVertex2d(city[nowcity].first*2,city[nowcity].second*2);
    }
    glVertex2d(city[G[0][0]].first*2,city[G[0][0]].second*2);
    glEnd();
    if(timer>180){
        if(Mincost_first==-2){
            Mincost_first++;
        }else if(Mincost_first==-1){
            Mincost_first=Fitness[0];
            Mincost_prev=Fitness[0];
                Mincost_prev2=Mincost_prev;
    }
    if(Mincost_prev!=Fitness[0]){
        Mincost_prev2=Mincost_prev;
        Mincost_prev=Fitness[0];
        }
    }
    std::string minc1="First Distance:";
    std::string minc2="Prev Distance:";        
    std::string str="Distance:";
    std::string gen="Generation:";
    str+=std::to_string(Fitness[0]);
    gen+=std::to_string(generation);
    minc1+=std::to_string(Mincost_first);
    minc2+=std::to_string(Mincost_prev2);        
    DrawString(str,500,500,300,250);
    DrawString(minc2,500,500,300,200);                
    DrawString(minc1,500,500,300,150);
    DrawString(gen,500,500,300,100);      
	glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble) h);
    width = w;
    height = h;
}
void idle(void)
{
    timer++;
    if(timer>180){
        if(generation<3000){
            //ココで遺伝子操作を行っている
                Calc();
                Mult();
                Mutation();
                generation++;
        }
    }
                glutPostRedisplay();

}
void init(void){
        glClearColor(0.0,0.0,0.0,0.0);
        //都市データ読み込み
        FILE *fp;
        if((fp=fopen("sample1.dat","r"))==NULL){
//                return -1;
        }

        fscanf(fp,"%d",&N);
        Loops(i,N){
                P c;
                fscanf(fp,"%lf %lf",&c.first,&c.second);
                city.push_back(c);
        }

        fclose(fp);
        init_greedy();

}

int main(int argc, char **argv)
{
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE);
        glutCreateWindow(argv[0]);
        glutDisplayFunc(display);
        glutReshapeFunc(reshape);
        glutIdleFunc(idle);
        init();
        glutMainLoop();
        return 0;
}
//ココからGA関係
void init_greedy(){
        srand(time(NULL));
        Loops(i,PARENTS){
                if(i<1){
                        std::vector<int>tmp;
                        Loops(k,N){
                                tmp.push_back(k);
                        }
                        G[i].push_back(rand()%N);
                        tmp.erase(tmp.begin()+G[i][0]);
                        int nowcity=G[i][0];
                        while(1){
                                int pivot=0;
                                int mincost=INF;
                                Loops(j,tmp.size()){
                                        if(mincost>distance(city[nowcity],city[tmp[j]])){
                                                mincost=distance(city[nowcity],city[tmp[j]]);
                                                pivot=j;
                                        }
                                }
                                nowcity=tmp[pivot];
                                G[i].push_back(pivot);
                                tmp.erase(tmp.begin()+pivot);
                                if(tmp.size()==0){
                                        break;
                                }
                        }
                }else {
                        Loops(j,N){
                                int randnum=rand()%(N-j);
                                G[i].push_back(randnum);
                        }
                }

        }
}
void Calc(){//距離計算&結果が良かった順ソート
        //srand(time(NULL));
        Loops(i,PARENTS){
                double sum=0;
                std::vector<int> tmp;
                Loops(k,N){
                        tmp.push_back(k);
                }
                int num_prev=tmp[G[i][0]];
                tmp.erase(tmp.begin()+G[i][0]);
                for(int j=1;j<N;j++){
                        int num=tmp[G[i][j]];
                        tmp.erase(tmp.begin()+G[i][j]);
                        sum+=distance(city[num_prev],city[num]);
                        num_prev=num;
                }
                sum+=distance(city[num_prev],city[G[i][0]]);
                Fitness[i]=sum;
        }
        Loops(i,PARENTS){
                for(int j=i;j<PARENTS;j++){
                        if(Fitness[i]>Fitness[j]){
                                std::swap(G[i],G[j]);
                                std::swap(Fitness[i],Fitness[j]);
                        }
                }
        }
}
void Mult(){//交叉
        std::vector<int> G_child[PARENTS*2];
        std::vector<double> F_inv(PARENTS);
        double sum=0;
        //適応度正規化(ルーレット方式で選択)
        Loops(i,PARENTS/2){
                F_inv[i]=1.0/Fitness[i];
                sum+=F_inv[i];
        }
        Loops(i,PARENTS/2){
                F_inv[i]/=sum;
                if(i!=0){
                        F_inv[i]+=F_inv[i-1];
                }
        }
        //トップ世代を次世代へ
        for(int i=0;i<ELITE;i++){
                G_child[i]=G[i];
        }
        //親世代から子世代作成
        for(int i=ELITE/2;i<(PARENTS/2);i++){
                //交叉する2個体を選出
                double random=(double)(rand()%10001)/10000.0;
                int p1,p2;
                Loops(j,PARENTS/2){
                        if(random<=F_inv[j]){
                                p1=j;
                                break;
                        }
                }
                while(1){
                        random=(double)(rand()%10001)/10000.0;
                        for(int j=0;j<PARENTS/2;j++){
                                if(random<=F_inv[j]){
                                        p2=j;
                                        break;
                                }
                        }
                        if(p1!=p2){
                                break;
                        }
                }
                //交叉(三つの交叉方法をそれぞれランダムで採用)
                int mult_random=rand()%3;
                if(mult_random==0){
                        mult_one_point(p1,p2);
                }else if(mult_random==1){
                        mult_two_points(p1,p2);
                }else {
                        mult_unif(p1,p2);
                }
                G_child[i*2]=Gtmp1;
                G_child[i*2+1]=Gtmp1;                           
        }
        //子世代を親世代へ
        Loops(i,PARENTS){
                G[i]=G_child[i];
        }
}
void Mutation(){//突然変異
        //突然変異にエリートは外しておく
        for(int i=ELITE;i<PARENTS;i++){
                Loops(j,N){
                        if(Prob(PROB_MUTATION)){
                                G[i][j]=rand()%(N-j);
                        }
                }
        }
}
void mult_unif(int x,int y){//一様交叉
        Gtmp1=G[x];
        Gtmp2=G[y];
        Loops(i,N){
                if(Prob(MULT_UNIF)){
                        std::swap(Gtmp1[i],Gtmp2[i]);           
                }
        }
}
void mult_one_point(int x,int y){//一点交叉
        Gtmp1=G[x];
        Gtmp2=G[y];
        int start=rand()%N;
        for(int i=start;i<N;i++){
                std::swap(Gtmp1[i],Gtmp2[i]);
        }
}
void mult_two_points(int x, int y){//二点交叉
        Gtmp1=G[x];
        Gtmp2=G[y];
        int random1=rand()%N;
        int random2=rand()%N;   
        int start=std::min(random1,random2);
        int end=std::max(random1,random2);
        for(int i=start;i<=end;i++){
                std::swap(Gtmp1[i],Gtmp2[i]);
        }
}
//汎用関数
double distance(P p1,P p2){
        return sqrt((p1.first-p2.first)*(p1.first-p2.first)+(p1.second-p2.second)*(p1.second-p2.second));
}
int Prob(double x){
        if((double)(rand()%10001)/10000.0<x){
                return 1;
        }else {
                return 0;
        }
}
static void DrawString(std::string str, int w, int h, int x0, int y0)
{
    glDisable(GL_LIGHTING);
    // 平行投影にする
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // 画面上にテキスト描画
    glRasterPos2f(x0, y0);
    int size = (int)str.size();
    for(int i = 0; i < size; ++i){
        char ic = str[i];
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, ic);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}