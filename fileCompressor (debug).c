#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define NUMBER_OF_BYTE 256
#define MAXBUF 16384
#define INFINITY -1
#define DEBUG
#undef DEBUG

struct send_data_2ndpart{
    unsigned int num;
};

struct coding_tree_node{
    unsigned int num;
    unsigned char character;
    unsigned int dum;
    struct coding_tree_node *left_child;
    struct coding_tree_node *right_child;
};


char code[NUMBER_OF_BYTE][256];
char tree_parse_result[256];
struct send_data_2ndpart package2[NUMBER_OF_BYTE];
struct coding_tree_node forest[NUMBER_OF_BYTE];

#ifdef DEBUG
char tmp_code[NUMBER_OF_BYTE][256];
int tree_depth=0;
#endif


void file_byte_variation_count(FILE *in,FILE *out);
void global_var_initialiaztion_to_compress();
void global_var_initialiaztion_to_expand();
void preprocess_forest();
void tree_creation();
void quick_sort(int ini_indx,int fin_indx);
void tree_parse(struct coding_tree_node *head,int depth);
void compress(FILE *in,FILE *out);
void expand(FILE *in,FILE *out);


int main(int argc,char *argv[])
{
    #ifdef DEBUG
        FILE *input,*output;
        int l,l1,ans;

        input=fopen("E:\\EULA.txt","rb");
        output=fopen("E:\\compress","wb");

        global_var_initialiaztion_to_compress();
        file_byte_variation_count(input,output);
        preprocess_forest();

        tree_creation();
        tree_parse(forest,0);

        compress(input,output);

        fclose(input);
        fclose(output);

        input=fopen("E:\\compress","rb");

        fseek(input,sizeof(unsigned int),SEEK_SET);
        fread(package2,sizeof(struct send_data_2ndpart),NUMBER_OF_BYTE,input);

        global_var_initialiaztion_to_expand();

        for(l=0;l<NUMBER_OF_BYTE;l++)
        {
            forest[l].num=package2[l].num;
        }

        preprocess_forest();

        for(l=0;l<NUMBER_OF_BYTE;l++)
        {
            for(l1=0;code[l][l1];l1++) tmp_code[l][l1]=code[l][l1];
            tmp_code[l][l1]=code[l][l1];
        }

        tree_creation();
        tree_parse(forest,0);

        for(l=0,ans=0;l<NUMBER_OF_BYTE;l++)
        {
            if(strcmp(code[l],tmp_code[l]) && package2[l].num){
                printf("%d %s %s\n",l,tmp_code[l],code[l]);
                ans++;
            }
        }

        printf("%d %d\n",ans,tree_depth);

        return 0;
    #endif
    if(argc==4){
        FILE *input,*output;

        if(argv[1][0]=='-'){
            input=fopen(argv[2],"rb");
            output=fopen(argv[3],"wb");

            if(input && output){
                global_var_initialiaztion_to_compress();
                file_byte_variation_count(input,output);
                preprocess_forest();

                tree_creation();
                tree_parse(&forest[0],0);

                compress(input,output);

                fclose(input);
                fclose(output);
            }
            else{
                puts("Fail to open i/o file");
            }
        }
        else if(argv[1][0]=='+'){
            input=fopen(argv[2],"rb");
            output=fopen(argv[3],"wb");

            if(input && output){
                int l;

                fseek(input,sizeof(unsigned int),SEEK_SET);
                fread(package2,sizeof(struct send_data_2ndpart),NUMBER_OF_BYTE,input);

                global_var_initialiaztion_to_expand();

                preprocess_forest();

                tree_creation();

                expand(input,output);

                fclose(input);
                fclose(output);
            }
            else{
                puts("Fail to open i/o file");
            }
        }
        else{
            puts("Wrong cmd format");
        }
    }

    return 0;
}

void global_var_initialiaztion_to_compress()
{
    int l;

    for(l=0;l<NUMBER_OF_BYTE;l++)
    {
        forest[l].dum=0;
        forest[l].character=l;
        forest[l].num=0;
        forest[l].left_child=NULL;
        forest[l].right_child=NULL;
        package2[l].num=0;
    }
}

void global_var_initialiaztion_to_expand()
{
    int l;

    for(l=0;l<NUMBER_OF_BYTE;l++)
    {
        forest[l].dum=0;
        forest[l].character=l;
        forest[l].num=package2[l].num;
        forest[l].left_child=NULL;
        forest[l].right_child=NULL;
    }
}

void file_byte_variation_count(FILE *in,FILE *out)
{
    unsigned char tmp[MAXBUF];
    unsigned int size=0;
    int l,l1;

    do
    {

        l=fread(tmp,1,MAXBUF,in);
        size+=l;

        for(l1=0;l1<l;l1++)
        {
            forest[tmp[l1]].num++;
            package2[tmp[l1]].num++;
        }

    }while(!feof(in));

    fwrite(&size,sizeof(unsigned int),1,out);
}

void preprocess_forest()
{
    int l;
    for(l=0;l<NUMBER_OF_BYTE;l++)
    {
        if(!forest[l].num) forest[l].num=INFINITY;
    }
}

void tree_creation()
{
    struct coding_tree_node tmp;

    quick_sort(0,NUMBER_OF_BYTE-1);

    do
    {
        tmp=forest[0];
        forest[0].dum=1;
        forest[0].num+=forest[1].num;
        forest[0].left_child=(struct coding_tree_node *)malloc(sizeof(struct coding_tree_node));
        forest[0].right_child=(struct coding_tree_node *)malloc(sizeof(struct coding_tree_node));
        *(forest[0].left_child)=tmp;
        *(forest[0].right_child)=forest[1];
        forest[1].num=INFINITY;

        quick_sort(0,NUMBER_OF_BYTE-1);

    }while(forest[1].num!=INFINITY);

}

void quick_sort(int ini_indx,int fin_indx)
{
    if(ini_indx>fin_indx) return;

    int l1=ini_indx-1,l;
    struct coding_tree_node tmp;

    for(l=ini_indx;l<fin_indx;l++)
    {
        if(forest[l].num<forest[fin_indx].num){
            l1++;
            tmp=forest[l];
            forest[l]=forest[l1];
            forest[l1]=tmp;
        }
    }
    l1++;
    tmp=forest[l];
    forest[l]=forest[l1];
    forest[l1]=tmp;

    quick_sort(ini_indx,l1-1);
    quick_sort(l1+1,fin_indx);

}

void tree_parse(struct coding_tree_node *head,int depth)
{
    if(head){

        if(head->dum){
            tree_parse_result[depth]='0';
            tree_parse(head->left_child,depth+1);

            tree_parse_result[depth]='1';
            tree_parse(head->right_child,depth+1);
        }
        else{
            int l;
            #ifdef DEBUG
            if(depth>tree_depth) tree_depth=depth;
            #endif // DEBUG
            tree_parse_result[depth]='\0';
            for(l=0;l<=depth;l++) code[head->character][l]=tree_parse_result[l];
        }
    }
}

void compress(FILE *in,FILE *out)
{
    unsigned char tmpin[MAXBUF],tmpout[MAXBUF];
    unsigned char tmpc1=0;
    unsigned int byte_to_write=0;
    unsigned int number_of_bufbytes_to_compress,cur_byte_to_compress,write_bit_pos=0,read_bit_pos=0;

    fseek(in,0,SEEK_SET);
    fwrite(package2,sizeof(package2),1,out);

    do{
        number_of_bufbytes_to_compress=fread(tmpin,1,MAXBUF,in);

        for(cur_byte_to_compress=0;cur_byte_to_compress<number_of_bufbytes_to_compress;)
        {

            for(;write_bit_pos<8;)
            {
                if(code[tmpin[cur_byte_to_compress]][read_bit_pos]=='1') tmpc1|=(1<<write_bit_pos),write_bit_pos++,read_bit_pos++;
                else if(code[tmpin[cur_byte_to_compress]][read_bit_pos]=='0') write_bit_pos++,read_bit_pos++;
                else{
                    cur_byte_to_compress++,read_bit_pos=0;
                    if(cur_byte_to_compress==number_of_bufbytes_to_compress) break;
                }
            }

            if(write_bit_pos==8){
                if(!code[tmpin[cur_byte_to_compress]][read_bit_pos]) cur_byte_to_compress++,read_bit_pos=0;

                tmpout[byte_to_write]=tmpc1;
                byte_to_write++;
                tmpc1=0;
                write_bit_pos=0;
            }

            if(byte_to_write==MAXBUF){
                fwrite(tmpout,1,MAXBUF,out);
                byte_to_write=0;
            }
        }

    }while(!feof(in));

    if(write_bit_pos){               //this is needed to add last input byte's compression code in output file
        tmpout[byte_to_write]=tmpc1; //in case last byte's compression doesn't fulfil an output byte it will not be written in out file
        byte_to_write++;             //while expansion this program will create huge loss of bytes(up to 16383(inclusive))
    }
    fwrite(tmpout,1,byte_to_write,out);
}

void expand(FILE *in,FILE *out)
{
    unsigned char tmpin[MAXBUF],tmpout[MAXBUF];
    unsigned char tmpc1=0;
    unsigned int num_of_bufbyte_to_decode,cur_bufbyte_to_decode,read_bit_pos=0,completed_outbuf_byte=0;
    unsigned int byte_written,output_file_size;
    struct coding_tree_node *tmp=&forest[0];

    fseek(in,0,SEEK_SET);
    fread(&output_file_size,sizeof(unsigned int),1,in);
    fseek(in,sizeof(package2),SEEK_CUR);

    for(byte_written=0;byte_written<output_file_size;)
    {
        num_of_bufbyte_to_decode=fread(tmpin,1,MAXBUF,in);

        for(cur_bufbyte_to_decode=0;cur_bufbyte_to_decode<num_of_bufbyte_to_decode;)
        {

            while(tmp->dum && read_bit_pos<8)
            {
                if(tmpin[cur_bufbyte_to_decode]&(1<<read_bit_pos)) tmp=tmp->right_child;
                else tmp=tmp->left_child;
                read_bit_pos++;
            }

            if(!tmp->dum){
                #ifdef DEBUG
                printf("%c %d %d %d\n",tmp->character,tmp->dum,tmp->num,l2);
                #endif // DEBUG
                tmpout[completed_outbuf_byte]=tmp->character;
                completed_outbuf_byte++;
                tmp=&forest[0];

                if(completed_outbuf_byte==MAXBUF || completed_outbuf_byte==output_file_size-byte_written){

                    byte_written+=fwrite(tmpout,1,completed_outbuf_byte,out);
                    //printf("%d %d\n",completed_outbuf_byte,output_file_size-byte_written);
                    //byte_written+=completed_outbuf_byte;
                    completed_outbuf_byte=0;

                    if(byte_written==output_file_size) break;
                }
            }

            if(read_bit_pos==8) read_bit_pos=0,cur_bufbyte_to_decode++;

        }
        printf("%d %d\n",completed_outbuf_byte,byte_written);
    }

}
