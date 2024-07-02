#include <stdio.h>
#include <iomanip>
#include <complex>
#include <map>
#include <unistd.h>
#include "sv_sub.hpp"
#include "go_pub.hpp"
#include "go_sub.hpp"
#include <fstream>
#include <iostream>
#include <deque>

#include "sv_go_comtrade.hpp"

const float TI_satur=500.0;
struct Medidas_MU 
{
    float medidas_DC[8];
    int SampleCount;
    float medidas_DC_prom[3];
};

        
#define NUM_DIG_COMTRADE 31
struct ddata_wrapper
{
    bool ddata[NUM_DIG_COMTRADE];
};

class fault_det 
{
	public:

    

    fault_det(){}

	// Setting definition
	const float I_diff_POS_sett=27; //A
	const float I_diff_NEG_sett=27; //A
	const double Time_delay_SS=78.125; //us
	const float deriv_I_POS_sett=0.04; //A/s
    const float deriv_I_NEG_sett=0.04; //A/s
    const float limit_satur_pos=500.0;//A
	const float limit_satur_neg=-500.0;//A

    
		
	
    void new_gopdu_data(const char* goID, const char* dateTime, const bool* value, const uint32_t* q, int stNum, int sqNum)
    {

       const std::lock_guard<std::mutex> lock(deque_mutex);
	
      bool CB_POS_SV_status_ant  = CB_POS_SV_status;
	    bool CB_NEG_SV_status_ant  = CB_NEG_SV_status;
	    bool CB_POS_PV_status_ant  = CB_POS_PV_status;
	    bool CB_NEG_PV_status_ant  = CB_NEG_PV_status;
	    bool CB_POS_Bat_status_ant  = CB_POS_Bat_status;
	    bool CB_NEG_Bat_status_ant  = CB_NEG_Bat_status;

       if (stNum != stNum_ant+1 && sqNum ==0  && stNum_ant >=0)
       {
                std::cout<<"lost st " << stNum_ant+1 << " received "<<stNum << "  (sq)" << sqNum<<std::endl;
       }
       


	
        std::cout<<"recgoose " << stNum<< "  " <<
        value[0] << " "<<
     		value[1] << " "<<
        value[2] << " "<<
        value[3] << " "<<
        value[4] << " "<<
        value[5] << " "<<
        value[6] <<std::endl;

        CB_POS_SV_status=value[0];
        CB_NEG_SV_status=value[1];
        CB_POS_PV_status=value[2];
        CB_NEG_PV_status=value[3];
        CB_POS_Bat_status=value[4];  
        CB_NEG_Bat_status=value[5];
        FALTA_TEST=value[6];
        
        if(CB_POS_SV_status && !CB_POS_SV_status_ant )
            cont_inicial_SV_POS = 0;
        if(CB_NEG_SV_status && !CB_NEG_SV_status_ant )
            cont_inicial_SV_NEG = 0;
        if(CB_POS_PV_status && !CB_POS_PV_status_ant )
            cont_inicial_PV_POS = 0;
        if(CB_NEG_PV_status && !CB_NEG_PV_status_ant )
            cont_inicial_PV_NEG = 0;
        if(CB_POS_Bat_status && !CB_POS_Bat_status_ant )
            cont_inicial_Bat_POS = 0;
        if(CB_NEG_Bat_status && !CB_NEG_Bat_status_ant )
            cont_inicial_Bat_NEG = 0;





    	stNum_ant =  stNum;
        //Falta=value[6];
       
        /*       Para verificar las entradas de la MU
        if ((CB_POS_SV_status==1)||(CB_NEG_SV_status==1))
            printf (" CB_POS_SV_status CB_NEG_SV_status %d %d\n",CB_POS_SV_status,CB_NEG_SV_status);
        if ((CB_POS_PV_status==1)||(CB_NEG_PV_status==1))    
            printf (" CB_POS_PV_status CB_NEG_PV_status, %d %d\n",CB_POS_PV_status, CB_NEG_PV_status);
        if ((CB_POS_Bat_status==1)||(CB_NEG_Bat_status==1)) 
            printf (" CB_POS_Bat_status CB_NEG_Bat_status, %d %d\n",CB_POS_Bat_status, CB_NEG_Bat_status);
        */

    }

    void new_asdu_data(const char* svID, const float* value, const uint32_t* q, int smpCnt)
	{
        

        if (smpCnt != smpCnt_ant+1 && smpCnt!=0 && smpCnt_ant >=0 )
        {
            std::cout<<"lost sc " << smpCnt_ant+1 << " received "<<smpCnt<<std::endl;
        }   
        smpCnt_ant= smpCnt;

        Medidas_MU Medidas_MU_SV;

        for (int i=0;i<4;i++)
        {
            Medidas_MU_SV.medidas_DC[i]=0.001*value[i];
            

            if ((Medidas_MU_SV.medidas_DC[i]>0)&&(Medidas_MU_SV.medidas_DC[i]>TI_satur))
            {
                Medidas_MU_SV.medidas_DC[i]=TI_satur;
            }
            else
            {
                if ((Medidas_MU_SV.medidas_DC[i]<0)&&(abs(Medidas_MU_SV.medidas_DC[i])>TI_satur))
                {
                    Medidas_MU_SV.medidas_DC[i]=-TI_satur;
                } 
            }
        }
    
        for (int i=4;i<8;i++)
        {
            Medidas_MU_SV.medidas_DC[i]=0.00001*value[i];

        }
        
        /*if (svID[3]=='2')
          printf (" svID I_POS_SV I_POS_PV I_POS_Bat %d %f %f %f\n",svID, Medidas_MU_SV.medidas_DC[0],Medidas_MU_SV.medidas_DC[1],Medidas_MU_SV.medidas_DC[2]);
        else
          printf (" svID I_NEG_SV I_NEG_PV I_NEG_Bat %d %f %f %f\n",svID, Medidas_MU_SV.medidas_DC[0],Medidas_MU_SV.medidas_DC[1],Medidas_MU_SV.medidas_DC[2]);        
        */

        Medidas_MU_SV.SampleCount=smpCnt;


        insert_data(svID,Medidas_MU_SV);  
        
   
         // Reseteo entrada 

        

	}
	
    
	private:

    bool static liberar()
    {
        bool liberar_SV_POS=Cierre_Intor(CB_POS_SV_status, cont_inicial_SV_POS);
        bool liberar_PV_POS=Cierre_Intor(CB_POS_PV_status, cont_inicial_PV_POS);
        bool liberar_Bat_POS=Cierre_Intor(CB_POS_Bat_status, cont_inicial_Bat_POS);
        bool liberar_SV_NEG=Cierre_Intor(CB_NEG_SV_status, cont_inicial_SV_NEG);
        bool liberar_PV_NEG=Cierre_Intor(CB_NEG_PV_status, cont_inicial_PV_NEG);
        bool liberar_Bat_NEG=Cierre_Intor(CB_NEG_Bat_status, cont_inicial_Bat_NEG);
        bool Enabled_old = Enabled;
        Enabled = (liberar_SV_POS&&liberar_PV_POS&&liberar_Bat_POS&&liberar_SV_NEG&&liberar_PV_NEG&&liberar_Bat_NEG);
        if(Enabled_old != Enabled)
            printf("liberar %s\n", Enabled ? "true": "false");
 
        return Enabled;
    }

    void static espera_breaker()
    {

	    bool breakers_open=false;
        

	    if(Falta_Diff &&
			    !CB_POS_SV_status &&
			    !CB_NEG_SV_status &&
			    !CB_POS_PV_status &&
			    !CB_NEG_PV_status &&
			    !CB_POS_Bat_status &&
			    !CB_NEG_Bat_status
			    )
	        breakers_open=true;

	    if(Falta_circuito_SV &&
			    !CB_POS_SV_status &&
			    !CB_NEG_SV_status 
			    )
	        breakers_open=true;



	    if(Falta_circuito_PV &&
			    !CB_POS_PV_status &&
			    !CB_NEG_PV_status 
			    )
	        breakers_open=true;

	    if(Falta_circuito_Bat &&
			    !CB_POS_Bat_status &&
			    !CB_NEG_Bat_status 
			    )
	        breakers_open=true;
                 
	    if(breakers_open)
                send_end_trip();
                
    }
 

    void static localiza_falta()
    {

    	//const float umbral = 0.1f;
	    const float umbral = 26.0f;

        const float *Int_Promedio_pos = inform_MU_POS_pre.back().medidas_DC_prom;
        const float *Int_Promedio_neg = inform_MU_NEG_pre.back().medidas_DC_prom;

	    const int num_samples_diff=6;

        //printf("Cjheck localiza0\n");
	    if (inform_MU_POS_pre.size() < num_samples_diff)
		    return;
        const float *Int_Promedio_pos_ant = inform_MU_POS_pre[inform_MU_POS_pre.size()-num_samples_diff].medidas_DC_prom ;

	    if (inform_MU_NEG_pre.size() < num_samples_diff)
		    return;
        const float *Int_Promedio_neg_ant = inform_MU_NEG_pre[inform_MU_NEG_pre.size()-num_samples_diff].medidas_DC_prom ;
  
        if(Int_Promedio_pos_ant[0]==0 && Int_Promedio_pos_ant[1]==0 && Int_Promedio_pos_ant[2]==0)
            return;
        if(Int_Promedio_neg_ant[0]==0 && Int_Promedio_neg_ant[1]==0 && Int_Promedio_neg_ant[2]==0)
            return;


      
        /*std::cout<<"pos: "<<abs(Int_Promedio_pos[0]-Int_Promedio_pos_ant[0]) << " "<<
	    abs(Int_Promedio_pos[1]-Int_Promedio_pos_ant[1]) << " "<<
	    abs(Int_Promedio_pos[2]-Int_Promedio_pos_ant[2]) << std::endl;
        std::cout<<"neg: "<<abs(Int_Promedio_neg[0]-Int_Promedio_neg_ant[0]) << " "<< 
	    abs(Int_Promedio_neg[1]-Int_Promedio_neg_ant[1]) << " "<<
	    abs(Int_Promedio_neg[2]-Int_Promedio_neg_ant[2]) << std::endl;*/
/*
       std::cout<<"pos1: "<<(Int_Promedio_pos[0]) << " " <<(Int_Promedio_neg[0]) << " "<<
       (Int_Promedio_pos[1]) << " " <<(Int_Promedio_neg[1]) << " "<<
       (Int_Promedio_pos[2]) << " " <<(Int_Promedio_neg[2]) << std::endl;
*/



     /*float umbral_absoluto = 18;
       if(
            abs(Int_Promedio_pos[0])<umbral_absoluto ||
            abs(Int_Promedio_pos[1])<umbral_absoluto ||
            abs(Int_Promedio_pos[2])<umbral_absoluto ||
            abs(Int_Promedio_neg[0])<umbral_absoluto ||
            abs(Int_Promedio_neg[1])<umbral_absoluto ||
            abs(Int_Promedio_neg[2])<umbral_absoluto &&false 
         )
         {

             std::cout<<"pos1: "<<abs(Int_Promedio_pos[0]) << " " <<abs(Int_Promedio_neg[0]) << " "<<
             abs(Int_Promedio_pos[1]) << " " <<abs(Int_Promedio_neg[1]) << " "<<
             abs(Int_Promedio_pos[2]) << " " <<abs(Int_Promedio_neg[2]) << std::endl;


             printf("Umbral Bajo -> Falta\n");
             Status=FAULT_DETECTED;
	         Falta_Diff = 2;

             printf("Create Comtrade\n");
	         comtrade = new Comtrade("Tigon",50, 12800, achannels, dchannels);
	         samples_post_trigger=0;
	         for(int i=0;i<inform_MU_POS_pre.size();i++)
	         {
                 if(i == 0)
                    comtrade->setStart(DateFromSmpCnt(inform_MU_POS_pre[i].SampleCount, 12800));
	             if(i == inform_MU_POS_pre.size()-1)
	             {
                     comtrade->setTrigger(DateFromSmpCnt(inform_MU_POS_pre[i].SampleCount, 12800));
	             }

                 insert_data_comtrade(inform_MU_POS_pre[i], inform_MU_NEG_pre[i]);
	         }
             return;
         }*/


	   
        bool POS_SV_diff_trigg = abs(Int_Promedio_pos[0]-Int_Promedio_pos_ant[0])>umbral;
        bool POS_PV_diff_trigg = abs(Int_Promedio_pos[1]-Int_Promedio_pos_ant[1])>umbral;
        bool POS_Bat_diff_trigg= abs(Int_Promedio_pos[2]-Int_Promedio_pos_ant[2])>umbral;

	     if (!POS_SV_diff_trigg || !POS_PV_diff_trigg ||!POS_Bat_diff_trigg)
        {
            //printf("nopos\n");
		    return;
        }
        

        bool SV_POS = (Int_Promedio_pos[0]-Int_Promedio_pos_ant[0])>0;
        bool PV_POS = (Int_Promedio_pos[1]-Int_Promedio_pos_ant[1])>0;
        bool Bat_POS= (Int_Promedio_pos[2]-Int_Promedio_pos_ant[2])>0;



        bool NEG_SV_diff_trigg = abs(Int_Promedio_neg[0]-Int_Promedio_neg_ant[0])>umbral;
        bool NEG_PV_diff_trigg = abs(Int_Promedio_neg[1]-Int_Promedio_neg_ant[1])>umbral;
        bool NEG_Bat_diff_trigg= abs(Int_Promedio_neg[2]-Int_Promedio_neg_ant[2])>umbral;

	   
	
	     if (!NEG_SV_diff_trigg || !NEG_PV_diff_trigg ||!NEG_Bat_diff_trigg)
        { 
            //printf("noneg\n");
		    return;
        }
        

        bool SV_NEG = (Int_Promedio_neg[0]-Int_Promedio_neg_ant[0])>0;
        bool PV_NEG = (Int_Promedio_neg[1]-Int_Promedio_neg_ant[1])>0;
        bool Bat_NEG= (Int_Promedio_neg[2]-Int_Promedio_neg_ant[2])>0;

        Status=FAULT_DETECTED;

        std::cout<<"pos: "<<(Int_Promedio_pos[0]-Int_Promedio_pos_ant[0]) << " "<<
	    (Int_Promedio_pos[1]-Int_Promedio_pos_ant[1]) << " "<<
	    (Int_Promedio_pos[2]-Int_Promedio_pos_ant[2]) << std::endl;
        std::cout<<"neg: "<<(Int_Promedio_neg[0]-Int_Promedio_neg_ant[0]) << " "<< 
	    (Int_Promedio_neg[1]-Int_Promedio_neg_ant[1]) << " "<<
	    (Int_Promedio_neg[2]-Int_Promedio_neg_ant[2]) << std::endl;
        Falta_Diff = 0;
	    if(SV_POS && PV_POS && Bat_POS && !SV_NEG && !PV_NEG && !Bat_NEG)
	        Falta_Diff = 3;
	    else if(SV_POS && PV_POS && Bat_POS)
	        Falta_Diff = 1;
	    else if(!SV_NEG && !PV_NEG && !Bat_NEG)
	        Falta_Diff = 2;

	    Falta_circuito_SV = 0;
	    if(!SV_POS && PV_POS && Bat_POS &&
	       !SV_NEG && PV_NEG && Bat_NEG)
	        Falta_circuito_SV = 1;
	    else if(SV_POS && !PV_POS && !Bat_POS &&
	        SV_NEG && !PV_NEG && !Bat_NEG)
	        Falta_circuito_SV = 2;
	    else if(!SV_POS && PV_POS && Bat_POS &&
	         SV_NEG && !PV_NEG && !Bat_NEG)
	        Falta_circuito_SV = 3;

	    Falta_circuito_PV = 0;
	    if(SV_POS && !PV_POS && Bat_POS &&
	       !SV_NEG && PV_NEG && Bat_NEG)
	        Falta_circuito_PV = 1;
	    else if(SV_POS && !PV_POS && !Bat_POS &&
	        !SV_NEG && PV_NEG && !Bat_NEG)
	        Falta_circuito_PV = 2;
	    else if(SV_POS && !PV_POS && Bat_POS &&
	        !SV_NEG && PV_NEG && !Bat_NEG)
	        Falta_circuito_PV = 3;


	    Falta_circuito_Bat = 0;
	    if(SV_POS && PV_POS && !Bat_POS &&
	        !SV_NEG && PV_NEG && Bat_NEG)
	        Falta_circuito_Bat = 1;
	    else if(SV_POS && !PV_POS && !Bat_POS &&
	        !SV_NEG && !PV_NEG && Bat_NEG)
	        Falta_circuito_Bat = 2;
	    else if(SV_POS && PV_POS && !Bat_POS &&
	        !SV_NEG && !PV_NEG && Bat_NEG)
	        Falta_circuito_Bat = 3;
     
      
        printf("Falta -> Diff %d SV %d PV %d Bat %d\n", Falta_Diff, Falta_circuito_SV, Falta_circuito_PV, Falta_circuito_Bat);
        
	    if(comtrade)
    	{
            std::string s=comtrade->getTrigger();
            std::replace( s.begin(), s.end(), '/', '-'); 
            std::replace( s.begin(), s.end(), ',', '_'); 
            std::replace( s.begin(), s.end(), ':', '_'); 
		    printf("Close Comtrade -> %s\n", s.c_str());
            comtrade->writeFiles(s);
		    delete comtrade;
    	}

        printf("Create Comtrade\n");
	    comtrade = new Comtrade("Tigon",50, 12800, achannels, dchannels);
	    samples_post_trigger=0;
	    for(int i=0;i<inform_MU_POS_pre.size();i++)
	    {
            if(i == 0)
                comtrade->setStart(DateFromSmpCnt(inform_MU_POS_pre[i].SampleCount, 12800));
	        if(i == inform_MU_POS_pre.size()-1)
	        {
                comtrade->setTrigger(DateFromSmpCnt(inform_MU_POS_pre[i].SampleCount, 12800));
	        }

            insert_data_comtrade(inform_MU_POS_pre[i], inform_MU_NEG_pre[i],ddata_pre[i].ddata);
	    }
    }

    static void fill_ddata(bool *ddata)
    {
        //TODO: All data 
	    ddata[0] = Enabled ;
	    ddata[1] = (Falta_Diff != 0) ;
        ddata[2] = (Falta_circuito_SV != 0) ;
        ddata[3] = (Falta_circuito_PV != 0) ;
        ddata[4] = (Falta_circuito_Bat != 0) ;
        ddata[5] = (Falta_Diff == 1) ;
        ddata[6] = (Falta_Diff == 2) ;
        ddata[7] = (Falta_Diff == 3) ;
        ddata[8] = (Falta_circuito_SV == 1) ;
        ddata[9] = (Falta_circuito_SV == 2) ;
        ddata[10] = (Falta_circuito_SV == 3) ;
        ddata[11] = (Falta_circuito_PV == 1) ;
        ddata[12] = (Falta_circuito_PV == 2) ;
        ddata[13] = (Falta_circuito_PV == 3) ;
        ddata[14] = (Falta_circuito_Bat == 1) ;
        ddata[15] = (Falta_circuito_Bat == 2) ;
        ddata[16] = (Falta_circuito_Bat == 3) ;
        ddata[17] = goose_values[0] ;
        ddata[18] = goose_values[1] ;
        ddata[19] = goose_values[2] ;
        ddata[20] = goose_values[3] ;
        ddata[21] = goose_values[4] ;
        ddata[22] = goose_values[5] ;
        ddata[23] = goose_values[6] ;
        ddata[24] = CB_POS_SV_status ;
        ddata[25] = CB_NEG_SV_status ;
        ddata[26] = CB_POS_PV_status ;
        ddata[27] = CB_NEG_PV_status ;
        ddata[28] = CB_POS_Bat_status;  
        ddata[29] = CB_NEG_Bat_status;
        ddata[30] = FALTA_TEST;
        
        

    }
    static void insert_data_comtrade(const Medidas_MU &pos, const Medidas_MU& neg, bool *ddata )
    {
        float adata[achannels.size()];

      adata[0] = pos.medidas_DC[0]; 
	    adata[1] = pos.medidas_DC[1]; 
	    adata[2] = pos.medidas_DC[2]; 
		        
	    adata[3] = neg.medidas_DC[0]; 
	    adata[4] = neg.medidas_DC[1]; 
	    adata[5] = neg.medidas_DC[2];

	    adata[6] = pos.medidas_DC_prom[0]; 
	    adata[7] = pos.medidas_DC_prom[1]; 
	    adata[8] = pos.medidas_DC_prom[2]; 
		        
	    adata[9] = neg.medidas_DC_prom[0]; 
	    adata[10] = neg.medidas_DC_prom[1]; 
	    adata[11] = neg.medidas_DC_prom[2];
     
     
        comtrade->addValue(adata,ddata);
    }
    static bool corriente_0()
    {
    	const float umbral = 0.1*180;//1.1*6.5f;
	    const float *Int_Promedio_pos = inform_MU_POS_pre.back().medidas_DC_prom;
        const float *Int_Promedio_neg = inform_MU_NEG_pre.back().medidas_DC_prom;

	    if(Falta_Diff)
	    {
            if(abs(Int_Promedio_pos[0])>umbral)
		        return false;
            if(abs(Int_Promedio_pos[1])>umbral)
		        return false;
	        if(abs(Int_Promedio_pos[2])>umbral)
		        return false;
            if(abs(Int_Promedio_neg[1])>umbral)
		        return false;
            if(abs(Int_Promedio_neg[2])>umbral)
		        return false;
	        if(abs(Int_Promedio_neg[3])>umbral)
		        return false;
	    }

        if(Falta_circuito_SV)
	    {
            if(abs(Int_Promedio_pos[0])>umbral)
		        return false;
            if(abs(Int_Promedio_neg[0])>umbral)
		        return false;

	    }	

        if(Falta_circuito_PV)
	    {
            if(abs(Int_Promedio_pos[1])>umbral)
		    return false;
            if(abs(Int_Promedio_neg[1])>umbral)
		    return false;

	    }

	    if(Falta_circuito_Bat)
	    {
            if(abs(Int_Promedio_pos[2])>umbral)
		    return false;
            if(abs(Int_Promedio_neg[2])>umbral)
		    return false;

	    }

        return true;
    }
    
    void static send_trip()
    {
        uint32_t qt[10]={0};

        goose_values[0] = Falta_Diff!=0 ;
        goose_values[1] = Falta_Diff!=0 || Falta_circuito_SV!=0;
        goose_values[2] = Falta_Diff!=0 || Falta_circuito_SV!=0;
        goose_values[3] = Falta_Diff!=0 || Falta_circuito_PV!=0;
        goose_values[4] = Falta_Diff!=0 || Falta_circuito_PV!=0;
        goose_values[5] = Falta_Diff!=0 || Falta_circuito_Bat!=0;
        goose_values[6] = Falta_Diff!=0 || Falta_circuito_Bat!=0;
        
        g_sender_MU2->setNewAllData(goose_values,qt);
        g_sender_MU3->setNewAllData(goose_values,qt);
        printf("Trip\n");

        Status = TRIP;
    }

    void static send_end_trip()
    {
        uint32_t qt[10]={0};

        goose_values[0] = 0 ;
        goose_values[1] = 0 ;
        goose_values[2] = 0 ;
        goose_values[3] = 0 ;
        goose_values[4] = 0 ;
        goose_values[5] = 0 ;
        goose_values[6] = 0 ;
        

        g_sender_MU2->setNewAllData(goose_values,qt);
        g_sender_MU3->setNewAllData(goose_values,qt);
        printf("End Trip\n");

        Status = FAULT_CHECK;
        Enabled = false;
        Falta_Diff = 0;
        Falta_circuito_SV = 0;
        Falta_circuito_PV = 0;
        Falta_circuito_Bat = 0;
        
    }
 
 
    static void func_promedio(const std::deque<Medidas_MU> &inform_MU, Medidas_MU &last_inform_MU)
    {
        
	    const int num_samples_mean = 4;
	    if(inform_MU.size()<num_samples_mean)
	    {
            last_inform_MU.medidas_DC_prom[0]=0;
            last_inform_MU.medidas_DC_prom[1]=0;
            last_inform_MU.medidas_DC_prom[2]=0;

	        return;

	    }

        float total_SV=last_inform_MU.medidas_DC[0];
        float total_PV=last_inform_MU.medidas_DC[1];
        float total_Bat=last_inform_MU.medidas_DC[2];


        
        for(int i=inform_MU.size()-1; i >= ((int)inform_MU.size()-num_samples_mean); i--)
        {
                total_SV=inform_MU[i].medidas_DC[0]+total_SV;
                total_PV=inform_MU[i].medidas_DC[1]+total_PV;
                total_Bat=inform_MU[i].medidas_DC[2]+total_Bat;
        }
        	//Size
        last_inform_MU.medidas_DC_prom[0]=total_SV/(num_samples_mean+1);
        last_inform_MU.medidas_DC_prom[1]=total_PV/(num_samples_mean+1);
        last_inform_MU.medidas_DC_prom[2]=total_Bat/(num_samples_mean+1);
	
	//printf("Prom SV %f %f\n",last_inform_MU.medidas_DC_prom[0], last_inform_MU.medidas_DC[0]);
    //printf("Prom PV %f %f\n",last_inform_MU.medidas_DC_prom[1], last_inform_MU.medidas_DC[1]);
    //printf("Prom Bat %f %f\n",last_inform_MU.medidas_DC_prom[2], last_inform_MU.medidas_DC[2]);



    }
/*
        static bool checkTimestamps(const std::deque<Medidas_MU> &positivo, const std::deque<Medidas_MU> &negativo, int &foundSampleCount)
        {
            auto it_positivo = positivo.end(); 
            auto it_negativo = negativo.end();

            for(auto it_pos = positivo.begin() ;it_pos != positivo.end();++it_pos)
            {

                auto it_neg = find_if(negativo.begin(),negativo.end(),[&] (const Medidas_MU& s) { return it_pos->SampleCount == s.SampleCount; });

                if (it_neg != negativo.end())
                {
                    foundSampleCount = it_pos->SampleCount;
                    it_positivo = it_pos;
                    it_negativo = it_neg;
                    break;
                }
            }

            if (it_positivo == positivo.end())
               return false;      
            if (it_negativo == negativo.end())
               return false;
           

            return true; 

        }
*/
        static bool checkTimestamps(const std::deque<Medidas_MU> &positivo, const std::deque<Medidas_MU> &negativo, int &foundSampleCount,std::deque<Medidas_MU> &inform_MU_POS,std::deque<Medidas_MU> &inform_MU_NEG)
        {
            auto it_positivo = positivo.end(); 
            auto it_negativo = negativo.end();

            for(auto it_pos = positivo.begin() ;it_pos != positivo.end();++it_pos)
            {

                auto it_neg = find_if(negativo.begin(),negativo.end(),[&] (const Medidas_MU& s) { return it_pos->SampleCount == s.SampleCount; });


                if (it_neg != negativo.end())
                {
                    foundSampleCount = it_pos->SampleCount;
                    it_positivo = it_pos;
                    it_negativo = it_neg;
                   break;
                }
            }

            if (it_positivo == positivo.end())
               return false;      
            if (it_negativo == negativo.end())
               return false;
           

            return true; 

        }

        
    static bool Cierre_Intor(bool CB_status, int &cont_inicial)
    {
        if(CB_status)
        {
                if ((cont_inicial+1)>=100)
                {
                    cont_inicial=100;
                    return true;
                }
                else
                {
                    cont_inicial=cont_inicial+1;
                }
        }
        
        return false;
    } 
         
    static void discardOldDCValues(std::deque<Medidas_MU> &terminal,int foundSampleCount)
    {
        auto it_r = find_if(terminal.begin(), terminal.end(),[&] (const Medidas_MU& s) { return foundSampleCount == s.SampleCount; });
        if (it_r!=terminal.begin())
            terminal.erase(terminal.begin(), it_r);
         
    }
    
    static void insert_data(const char* svID,Medidas_MU &Medidas_MU_SV)
    {
        const std::lock_guard<std::mutex> lock(deque_mutex);
	    const int num_samples_pre = 1000;//24;
	    const int num_samples_post_trigger = 12800;

        if (svID[3]=='2') //Positivo
        {
            inform_MU_POS.push_back(Medidas_MU_SV);
        }
        else
        {
            inform_MU_NEG.push_back(Medidas_MU_SV);
        }
            
        //printf("Cjheck ts %d %d\n", inform_MU_POS.size(), inform_MU_NEG.size());
        int foundSampleCount=0;
        if (checkTimestamps(inform_MU_POS,inform_MU_NEG,foundSampleCount,inform_MU_POS,inform_MU_NEG))
        {
            discardOldDCValues(inform_MU_POS,foundSampleCount);
            discardOldDCValues(inform_MU_NEG,foundSampleCount);

                
            Medidas_MU pos=inform_MU_POS.front();
            Medidas_MU neg=inform_MU_NEG.front();

            //printf("Promedio\n");
            func_promedio(inform_MU_POS_pre, pos);
            func_promedio(inform_MU_NEG_pre, neg);

            inform_MU_POS_pre.push_back(pos);
            if (inform_MU_POS_pre.size()>num_samples_pre)
                inform_MU_POS_pre.pop_front();

            inform_MU_NEG_pre.push_back(neg);
            if (inform_MU_NEG_pre.size()>num_samples_pre)
                inform_MU_NEG_pre.pop_front();
	                
            ddata_wrapper ddata;
            fill_ddata(ddata.ddata);

            ddata_pre.push_back(ddata);
            if (ddata_pre.size()>num_samples_pre)
                ddata_pre.pop_front();
                
                
	//#########################################################################################################################
            if (FALTA_TEST == 1)
            
              Status = FAULT_CHECK;
             // printf("LOCALIZO 1\n");
              
 	//#########################################################################################################################
              
            //static int ii=0;
		    if(Status == FAULT_CHECK /*&& ii++>10000 */&& liberar())
            
                localiza_falta();
  	//#########################################################################################################################
               // printf("LOCALIZO 2\n");
   	//#########################################################################################################################
               
            
			if(comtrade)
		    {
		        if(samples_post_trigger++ < num_samples_post_trigger && comtrade)
                {
		            insert_data_comtrade(pos, neg, ddata.ddata);
                }
		        else
		        {
                        std::string s=comtrade->getTrigger();
                        std::replace( s.begin(), s.end(), '/', '-'); 
                        std::replace( s.begin(), s.end(), ',', '_'); 
                        std::replace( s.begin(), s.end(), ':', '_'); 
			            printf("Close Comtrade -> %s\n", s.c_str());
                        comtrade->writeFiles(s);
			            delete comtrade;
			            comtrade = NULL;
		        }
		    }

		    if(Status == FAULT_DETECTED)
                if(corriente_0())
                    send_trip(); 
		    if(Status == TRIP)
                    espera_breaker(); 

            inform_MU_POS.pop_front(); 
            inform_MU_NEG.pop_front(); 
        }
    }
	public:
    static void set_goose_senders(go_sender* go2, go_sender*go3)
	{
            g_sender_MU2 = go2;
            g_sender_MU3 = go3;
	}
    static void init_comtrade()
	{
        int channel_index=0;
        achannels.push_back(AnalogChannelInfo(channel_index++, "I_SV_POS", "", "A", 1, 0, 1, 1, true));
        achannels.push_back(AnalogChannelInfo(channel_index++, "I_PV_POS", "", "A", 1, 0, 1, 1, true));
        achannels.push_back(AnalogChannelInfo(channel_index++, "I_Bat_POS", "", "A", 1, 0, 1, 1, true));

        achannels.push_back(AnalogChannelInfo(channel_index++, "I_SV_NEG", "", "A", 1, 0, 1, 1, true));
        achannels.push_back(AnalogChannelInfo(channel_index++, "I_PV_NEG", "", "A", 1, 0, 1, 1, true));
        achannels.push_back(AnalogChannelInfo(channel_index++, "I_Bat_NEG", "", "A", 1, 0, 1, 1, true));

	      achannels.push_back(AnalogChannelInfo(channel_index++, "I_SV_POS_Prom", "", "A", 1, 0, 1, 1, true));
        achannels.push_back(AnalogChannelInfo(channel_index++, "I_PV_POS_Prom", "", "A", 1, 0, 1, 1, true));
        achannels.push_back(AnalogChannelInfo(channel_index++, "I_Bat_POS_Prom", "", "A", 1, 0, 1, 1, true));

        achannels.push_back(AnalogChannelInfo(channel_index++, "I_SV_NEG_Prom", "", "A", 1, 0, 1, 1, true));
        achannels.push_back(AnalogChannelInfo(channel_index++, "I_PV_NEG_Prom", "", "A", 1, 0, 1, 1, true));
        achannels.push_back(AnalogChannelInfo(channel_index++, "I_Bat_NEG_Prom", "", "A", 1, 0, 1, 1, true));


             
	    channel_index = 0;
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "Enabled"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "Bar"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "SV"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "PV"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "Bat"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "Bar1"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "Bar2"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "Bar3"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "SV1"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "SV2"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "SV3"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "PV1"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "PV2"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "PV3"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "Bat1"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "Bat2"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "Bat3"));
      dchannels.push_back(DigitalChannelInfo(channel_index++, "TRIP_DIFF"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "TRIP_SV_POS"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "TRIP_SV_NEG"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "TRIP_PV_POS"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "TRIP_PV_NEG"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "TRIP_Bat_POS"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "TRIP_Bat_NEG"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "CB_POS_SV_status"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "CB_NEG_SV_status"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "CB_POS_PV_status"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "CB_NEG_PV_status"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "CB_POS_Bat_status"));
	    dchannels.push_back(DigitalChannelInfo(channel_index++, "CB_NEG_Bat_status"));
      dchannels.push_back(DigitalChannelInfo(channel_index++, "Det_Falta"));


	}
	private:
        static go_sender *g_sender_MU2;
        static go_sender *g_sender_MU3;
        static std::deque<Medidas_MU> inform_MU_POS;
        static std::deque<Medidas_MU> inform_MU_NEG;
        static std::deque<Medidas_MU> inform_MU_POS_pre;
        static std::deque<Medidas_MU> inform_MU_NEG_pre;
        static std::deque<ddata_wrapper> ddata_pre;
        static bool goose_values[10];

        static Comtrade *comtrade;
        static std::vector<AnalogChannelInfo> achannels;
        static std::vector<DigitalChannelInfo> dchannels;


        static std::mutex deque_mutex;
        int stNum_ant;

        int smpCnt_ant;
        // Estado del interruptor
        static bool CB_POS_SV_status;
        static bool CB_POS_PV_status;
        static bool CB_POS_Bat_status;
        static bool CB_NEG_SV_status;
        static bool CB_NEG_PV_status;
        static bool CB_NEG_Bat_status;
        static bool FALTA_TEST;

        // señales del contador para el cierre del interruptor
        static int cont_inicial_SV_POS;
        static int cont_inicial_PV_POS;
        static int cont_inicial_Bat_POS;
        static int cont_inicial_SV_NEG;
        static int cont_inicial_PV_NEG;
        static int cont_inicial_Bat_NEG;

	enum fault_det_status
	{
		FAULT_CHECK,
		FAULT_DETECTED,
		TRIP
	};

	static fault_det_status Status;
	static bool Enabled;
	static int Falta_Diff;
	static int Falta_circuito_SV;
	static int Falta_circuito_PV;
	static int Falta_circuito_Bat;
    static int samples_post_trigger;

    std::ofstream archivo;
       
	std::chrono::nanoseconds st;

};

go_sender *fault_det::g_sender_MU2=NULL;
go_sender *fault_det::g_sender_MU3=NULL;
 

std::deque<Medidas_MU> fault_det::inform_MU_POS;
std::deque<Medidas_MU> fault_det::inform_MU_NEG;
std::deque<Medidas_MU> fault_det::inform_MU_POS_pre;
std::deque<Medidas_MU> fault_det::inform_MU_NEG_pre;
std::deque<ddata_wrapper> fault_det::ddata_pre;
bool fault_det::goose_values[10]={0};

std::mutex fault_det::deque_mutex;
bool fault_det::CB_POS_SV_status=false;
bool fault_det::CB_POS_PV_status=false;
bool fault_det::CB_POS_Bat_status=false;
bool fault_det::CB_NEG_SV_status=false;
bool fault_det::CB_NEG_PV_status=false;
bool fault_det::CB_NEG_Bat_status=false;
bool fault_det::FALTA_TEST=false;


int fault_det::cont_inicial_SV_POS=0;
int fault_det::cont_inicial_PV_POS=0;
int fault_det::cont_inicial_Bat_POS=0;
int fault_det::cont_inicial_SV_NEG=0;
int fault_det::cont_inicial_PV_NEG=0;
int fault_det::cont_inicial_Bat_NEG=0;

fault_det::fault_det_status fault_det::Status = fault_det::FAULT_CHECK;
bool fault_det::Enabled=false;
int  fault_det::Falta_Diff=0;
int  fault_det::Falta_circuito_SV=0;
int  fault_det::Falta_circuito_PV=0;
int  fault_det::Falta_circuito_Bat=0;
int  fault_det::samples_post_trigger=0;

Comtrade *fault_det::comtrade = NULL;
std::vector<AnalogChannelInfo> fault_det::achannels;
std::vector<DigitalChannelInfo> fault_det::dchannels;




using namespace std;


int main(int argc, char **argv) {


    std::string net_device = "eno2";
    int c;
    std::string filename1="configuration_tigon_MU2_POS_18102023.cid";
    std::string filename2="configuration_tigon_MU3_NEG_18102023.cid";
    //std::string go_filename1="configuration_tigon_MU2_POS_18102023.cid";
    std::string go_filename="configuration_tigon_18102023.cid";
    std::string ied_name;
    std::string control_block;

    



    sv_ied_parser ied_parser1;
    sv_ied_parser ied_parser2;
    ied_parser1.parse(filename1);    
    ied_parser2.parse(filename2);  

    sv_ied_config sv_config1;
    sv_ied_config sv_config2;
    int n_configs1=ied_parser1.get_unique_sv_config(ied_name, control_block, &sv_config1);
    if(n_configs1!=1)
    {
	    std::cout<<"SV config found in "<<filename1<< " -> "<<std::endl;
	    ied_parser1.print();
            std::cout<<"Not unique Control Block "<<control_block<< " (iedName ->"<<ied_name<< ") " << " not found in "<< filename1 << " file. N blocks -> "<<n_configs1<< ". Abort."<<std::endl;
	    exit(1);
    }

    int n_configs2=ied_parser2.get_unique_sv_config(ied_name, control_block, &sv_config2);
    if(n_configs2!=1)
    {
	    std::cout<<"SV config found in "<<filename2<< " -> "<<std::endl;
	    ied_parser2.print();
            std::cout<<"Not unique Control Block "<<control_block<< " (iedName ->"<<ied_name<< ") " << " not found in "<< filename2 << " file. N blocks -> "<<n_configs2<< ". Abort."<<std::endl;
	    exit(1);
    }

    std::cout<<"Selected SV Config ->"<<std::endl;
    sv_config1.print();
    sv_config2.print();

    go_ied_config go_config;
    go_ied_parser go_ied_parser;
    go_ied_parser.parse(go_filename);
   
    
    int n_configs=go_ied_parser.get_unique_go_config(ied_name, "GOOSE_MU2", &go_config); // Para publicar TIGON MU2
    
    if(n_configs!=1)
    {
	    std::cout<<"GOOSE config found in "<<go_filename<< " -> "<<std::endl;
	    go_ied_parser.print();
	    std::cout<<"Not unique Control Block GOOSE_MU2 (iedName ->"<<ied_name<< ") " << " not found in "<< go_filename << " file. N blocks -> "<<n_configs<< ". Abort."<<std::endl;
	    exit(1);
    }
    std::cout<<"Selected GOOSE Config ->"<<std::endl;
    go_config.print();
 
    go_sender g_sender_MU2("eno4", go_filename.c_str(), go_config.ied_name.c_str(), go_config.cb_name.c_str() );
    g_sender_MU2.init();
    g_sender_MU2.start();
     
    
    n_configs=go_ied_parser.get_unique_go_config(ied_name, "GOOSE_MU3", &go_config); // Para publicar TIGON MU3
    
    if(n_configs!=1)
    {
	    std::cout<<"GOOSE config found in "<<go_filename<< " -> "<<std::endl;
	    go_ied_parser.print();
	    std::cout<<"Not unique Control Block GOOSE_MU3 (iedName ->"<<ied_name<< ") " << " not found in "<< go_filename << " file. N blocks -> "<<n_configs<< ". Abort."<<std::endl;
	    exit(1);
    }
    std::cout<<"Selected GOOSE Config ->"<<std::endl;
    go_config.print();
 
    go_sender g_sender_MU3("eno4", go_filename.c_str(), go_config.ied_name.c_str(), go_config.cb_name.c_str());
    g_sender_MU3.init();
    g_sender_MU3.start();


    go_ied_config go_config_r;
    go_ied_parser.parse(filename1);

    n_configs=go_ied_parser.get_unique_go_config(ied_name, "FastGOOSE1", &go_config_r);
    if(n_configs!=1)
    {
	    std::cout<<"GOOSE config found in "<<filename1<< " -> "<<std::endl;
	    go_ied_parser.print();
	    std::cout<<"Not unique Control Block FastGOOSE1 (iedName ->"<<ied_name<< ") " << " not found in "<< filename1 << " file. N blocks -> "<<n_configs<< ". Abort."<<std::endl;
	    exit(1);
    }

    std::cout<<"Selected GOOSE Config ->"<<std::endl;
    go_config_r.print();


    fault_det::init_comtrade();
    fault_det::set_goose_senders(&g_sender_MU2, &g_sender_MU3);

    fault_det fd1;
    fault_det fd2;

    go_subscriber<fault_det> go_sub(filename1.c_str(), go_config_r.ied_name.c_str(), go_config_r.cb_name.c_str() , "eno2" , &fd1);
    std::thread t_sub_goose(&go_subscriber<fault_det>::capture_loop, &go_sub);

    sv_subscriber<fault_det> sub1(sv_config1, net_device, &fd1);
    sv_subscriber<fault_det> sub2(sv_config2, net_device, &fd2);
    std::thread t_sub1(&sv_subscriber<fault_det>::capture_loop, &sub1); 
    std::thread t_sub2(&sv_subscriber<fault_det>::capture_loop, &sub2);
    
    t_sub_goose.join();
    g_sender_MU2.join();
    g_sender_MU3.join();

    t_sub1.join();
    t_sub2.join();


    return 0;
}

