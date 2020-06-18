/*
这个程序是每个进程单独搜一个树枝，然后得到自己本树枝的最好回路，然后一起发送给
process 0,process 0进行归约判断，看哪个的cost最小，就说明哪个是全局最佳


不足之处：mpi传递不合适，如果改成传递Tour对象会好点，可是tm的老师ppt上没有吧？醉了，这网课
真的无聊，不就是调用各种api吗？无聊无聊
8月份考试咋办，唉
*/
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<mpi.h>
int node_num=7;     //city number
typedef struct 
{
    int pre;
    int post;
    int cost;
}MAP_NODE;

typedef struct 
{
    int route[30];
    int total_cost;
}Tour;
typedef struct {
    Tour Data[1000];
    int Topidx;            //栈指针初始化为0,也就是永远指向最近空的栈元素
}STACK_Route;
int Push( STACK_Route * stack , Tour tour  ){
    if(IsFull(&stack)) return 0;
    stack->Data[stack->Topidx]=tour;
    stack->Topidx++;
    return 1;

}
Tour Pop(STACK_Route * stack){
   // if(IsEmpty(&stack)) return 0;
    stack->Topidx--;
    return stack->Data[stack->Topidx];
}
int  IsEmpty(STACK_Route * stack){
    if(stack->Topidx==0)
    return 1;
    else return 0;
}
int IsFull(STACK_Route* stack){
    if(stack->Topidx==1000-1)
    return 1;
    else return 0;
}
void Init_map(MAP_NODE map[])
{
  //  MAP_NODE map[1000];
    srand((int)time(0));
    int pre=0,post=0,cost=0;
    int i=0,j=0;
    for(pre=0,i=0;pre<node_num;pre++)
    {
        for( post=0;post<node_num;post++)
             {
                 if(post==pre) continue;
                cost=1+(int)(10.0*rand()/(RAND_MAX+1.0));   //产生0-10之内的随机数
                MAP_NODE node;
                node.cost=cost;
                node.post=post;
                node.pre=pre;
                map[i]=node;
                i++;
                //free(&node);
             }
    }
}
int Feasible(Tour curr_tour,int nbr ,MAP_NODE map[]){
    int i=0,j=0,k=0;
 
    for(i=0;;i++)
    {
        if(curr_tour.route[i]==-1)
        break;
    }
    i=i-1;   //现在走到的城市
    // printf("i=%d ",i);
    //    printf("%d %d ",curr_tour.route[i],nbr);
    if(curr_tour.route[i]==nbr) return 0;     //如果nbr和route的最后一个城市相同，则不可以形成通路
    for(j=0;j<i;j++)
    {
        if(curr_tour.route[j]==nbr) return 0;     //如果曾经走到过这个城市，则不可以形成通路
    }
    return 1;  //如果这个nbr之前没出现过，则判定是可以加入的，全联通图，肯定有路
    // for(j=0;j<node_num*(node_num-1);j++)
    // {
    //     if(map[j].pre==curr_tour.route[i])    //如果在地图中找到了这个现在走的城市
    //     {
    //         for(k=0;k<i;k++)
    //         {
    //             if(map[j].post==curr_tour.route[k])
    //             break;
    //         }
    //          if(k==i) return 1;
    //     }
    // }
}
void Init_stack(STACK_Route *stack){      //初始化栈
    stack->Topidx=0;
    int i=0,j=0;
    for(i=0;i<1000;i++)
    {
        stack->Data[i].total_cost=0;
        for(j=0;j<node_num;j++)
        stack->Data[i].route[j]=-1;
    }
}

int City_count(Tour curr_tour){
    int i=0,num=0;
    for(i=0;i<node_num;i++)
    {
        if(curr_tour.route[i]!=-1)
        num++;
    }
    return num;
}
int Best_tour(Tour curr_tour, Tour best_tour){    //if the current tour is the best

    if(curr_tour.total_cost<best_tour.total_cost)
    return 1;
    else return 0;
}

void  Update_best_tour(Tour curr_tour, Tour * best_tour){    

   
    best_tour->total_cost=curr_tour.total_cost;
    int i=0;
    for(i=0;i<node_num;i++)
    best_tour->route[i]=curr_tour.route[i];
}
void Add_city(Tour * curr_tour,int nbr,MAP_NODE map[]){
    int i=0,j=0;
     for(i=0;;i++)
    {
        if(curr_tour->route[i]==-1)
        break;
    }
    i=i-1;   //现在走到的城市
    for(j=0;j<node_num*(node_num-1);j++){
        if(map[j].pre==curr_tour->route[i]&& map[j].post==nbr)
        {
            curr_tour->total_cost+=map[j].cost;
            curr_tour->route[i+1]=nbr;
            break;
        }
    }
    //printf("find one %d to %d \n",map[j].pre,map[j].post);
    
}
void Remove_last_city(Tour * curr_tour){
    int i=0;
 
    for(i=0;;i++)
    {
        if(curr_tour->route[i]==-1)
        break;
    }
    i=i-1;   //现在走到的城市
    curr_tour->route[i]=-1;
}
int main( int argc, char *argv[] )
{
    Tour best_tour;
    best_tour.total_cost=9999999;
    MAP_NODE map[1000];
    Init_map(map);
    int i=node_num*(node_num-1),j=0,k=0;
    int nbr=0;
    STACK_Route stack;
    Init_stack(&stack);
    Tour curr_tour,tour;
    tour.total_cost=0;
    int rank, size;
    MPI_Init( &argc, &argv );
    MPI_Status status;
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    if(rank!=0){
    tour.route[0]=0;
    tour.route[1]=rank;
    for(k=2;k<node_num;k++)
    tour.route[k]=-1;
    Push(&stack,tour);
    while(!IsEmpty(&stack)){
        curr_tour=Pop(&stack);
        if(City_count(curr_tour)==node_num)
        {
            if(Best_tour(curr_tour,best_tour))
            Update_best_tour(curr_tour,&best_tour);
        }else{
                for(nbr=node_num-1;nbr>=1;nbr--)
                    if(Feasible(curr_tour,nbr,map))
                    {
                        Add_city(&curr_tour ,nbr,map);
                        Push(&stack,curr_tour);
                        Remove_last_city(&curr_tour);
                    }
        }
     }
   

   
    int fuck=0;
    printf("the best local tour is 0");
    for(fuck=1;fuck<node_num;fuck++)
    printf("->%d",best_tour.route[fuck]);
    printf(" and the best cost is %d from process %d \n",best_tour.total_cost,rank);
    int  local_cost=best_tour.total_cost;
    MPI_Send( &local_cost, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    if(rank==0)
    {
        //Tour alltout[node_num];
        int all_cost[node_num];
        int p=0,q=0,process=1;
        for(p=1;p<node_num;p++)
        MPI_Recv( &all_cost[p], 1, MPI_INT, p, 0, MPI_COMM_WORLD, &status );
        int min=all_cost[1];
        for(q=2;q<=p;q++)
        {
            if(all_cost[q]<min)
            {
            min=all_cost[q];
            process=q;
            }
        }    
        printf("\n!!!obviously the best cost is %d from process %d\n",min,process);
    
    
    
    }
    MPI_Finalize();
  //  printf("%d",City_count(curr_tour));
   // printf("%d %d ",curr_tour.route[0] ,curr_tour.route[2]);
    // for(j=0;j<i;j++)
    // printf("%d->%d cost is %d\n",map[j].pre,map[j].post,map[j].cost);
     //   printf("%d",stack.Topidx);
   // printf("%d %d %d " ,stack.Topidx,stack.Data[1].total_cost,stack.Data[199].route[12]);
    return 0;
}