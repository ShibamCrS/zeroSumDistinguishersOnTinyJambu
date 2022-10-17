#include "utility.h"

/*
 * Store the degree and the confirmed monomial
 */
struct DV
{
    int deg;
    Monomial vec;
};

void Core(GRBModel& model, vector<GRBVar>& x,int x5,int x4,int x3,int x2,int x1,int x0)
{    
   int Ineq[][13] = {
 {0,-1 ,-1 ,-1 ,0 ,-1 ,-1 ,1 ,1 ,1 ,0 ,1 ,1 },
{0,1 ,0 ,0 ,0 ,0 ,0 ,-1 ,0 ,0 ,0 ,0 ,0 },
{0,0 ,1 ,0 ,0 ,0 ,0 ,0 ,-1 ,0 ,0 ,0 ,0 },
{0,0 ,0 ,1 ,0 ,0 ,0 ,0 ,0 ,-1 ,0 ,0 ,0 },
{0,0 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,0 ,0 ,-1 ,0 },
{0,0 ,0 ,0 ,1 ,0 ,0 ,0 ,0 ,0 ,-1 ,0 ,0 },
{0,-1 ,-1 ,0 ,-1 ,-1 ,-1 ,1 ,1 ,0 ,1 ,1 ,1 },
{0,0 ,0 ,1 ,-1 ,0 ,0 ,0 ,0 ,0 ,1 ,0 ,0 },
{0,0 ,0 ,-1 ,1 ,0 ,0 ,0 ,0 ,1 ,0 ,0 ,0 },
{3,0 ,0 ,1 ,0 ,0 ,1 ,-1 ,-1 ,0 ,0 ,-1 ,-1 },
{1,0 ,1 ,1 ,0 ,1 ,1 ,-1 ,0 ,0 ,0 ,0 ,-1 },
{1,1 ,0 ,1 ,0 ,1 ,1 ,0 ,-1 ,0 ,0 ,0 ,-1 },
{1,1 ,1 ,0 ,1 ,0 ,1 ,0 ,0 ,0 ,0 ,-1 ,-1 },
{3,0 ,0 ,0 ,1 ,0 ,1 ,-1 ,-1 ,0 ,0 ,-1 ,-1 },
{4,0 ,0 ,0 ,0 ,1 ,1 ,-1 ,-1 ,-1 ,-1 ,0 ,-1 },
{2,1 ,1 ,0 ,0 ,1 ,1 ,0 ,0 ,-1 ,-1 ,0 ,-1 },
{1,1 ,0 ,0 ,1 ,1 ,1 ,0 ,-1 ,0 ,0 ,0 ,-1 },
{1,0 ,1 ,0 ,1 ,1 ,1 ,-1 ,0 ,0 ,0 ,0 ,-1 },
{1,1 ,1 ,1 ,0 ,0 ,1 ,0 ,0 ,0 ,0 ,-1 ,-1 },
{4,0 ,1 ,0 ,0 ,0 ,1 ,-1 ,0 ,-1 ,-1 ,-1 ,-1 },
{4,1 ,0 ,0 ,0 ,0 ,1 ,0 ,-1 ,-1 ,-1 ,-1 ,-1 }
 };
   
 GRBVar y0 = model.addVar(0, 1, 0, GRB_BINARY);
 GRBVar y1 = model.addVar(0, 1, 0, GRB_BINARY);
 GRBVar y2 = model.addVar(0, 1, 0, GRB_BINARY);
 GRBVar y3 = model.addVar(0, 1, 0, GRB_BINARY);
 GRBVar y4 = model.addVar(0, 1, 0, GRB_BINARY);
 GRBVar y5 = model.addVar(0, 1, 0, GRB_BINARY);
 
 for ( auto it : Ineq )
    model.addConstr( it[0] + it[1] * x[x5] + it[2] * x[x4] + it[3] * x[x3] + it[4] * x[x2] + it[5] * x[x1] + it[6] * x[x0] + it[7] * y5 + it[8] * y4 + it[9] * y3 +  it[10] * y2 + it[11] * y1 + it[12] * y0 >= 0 );

 x[x0] = y0;
 x[x1] = y1;
 x[x2] = y2;
 x[x3] = y3;
 x[x4] = y4;
 x[x5] = y5;
}
void SecondBackExpandPolynomial(int rounds, vector<Monomial> &Terms, 
						 Monomial last){
	GRBEnv env = GRBEnv();
	env.set(GRB_IntParam_LogToConsole, 0);
	env.set(GRB_IntParam_PoolSearchMode, 2);
	env.set(GRB_IntParam_PoolSolutions, MAX);
	
	GRBModel model = GRBModel( env );
	
	//declare the variables
	vector<GRBVar> s(STATE);
	for(int i=0; i<STATE; i++){
		s[i] = model.addVar(0,1,0,GRB_BINARY);
	} 
	
	//propagate the monomials
	vector<GRBVar> works = s;
	for(int r=0; r<rounds; r++){
		Core(model, works, 128, 81, 58, 43, 37, 0);
		vector<GRBVar> temp = works;
		//Value Rotation
        for (int j = 0; j < 128; j++) 
            works[j] = temp[(j+1) % 128];
        //Key Rotation
        //k_128........k_255
        for (int j = 0; j < 128; j++) 
            works[128 + j] = temp[128 + ((j+1) % 128)];
            
	}
	for ( int i = 0; i < STATE; i++ )
        if ( last[i] == 0 )
            model.addConstr( works[i] == 0 );
        else
            model.addConstr( works[i] == 1 );

    model.update();
    model.write("./data/second_tinyJambu_inverse_degree.lp");
    model.optimize();
    
    //Prepare the hash table for each solution
    map<Monomial, int, cmp> H;
    
    if( model.get( GRB_IntAttr_Status ) == GRB_OPTIMAL){
    	double time = model.get(GRB_DoubleAttr_Runtime );
        cout << "Time Used: " << time << "sec" << endl;
        
        int solCount = model.get(GRB_IntAttr_SolCount);
        cout << "Raw Solutions: " << solCount << endl;
       
        bitset<STATE> hash_key;
        for(int i=0; i<solCount; i++){
        	model.set(GRB_IntParam_SolutionNumber, i );
        	for ( int j = 0; j < STATE; j++ ) 
                if ( round( s[j].get( GRB_DoubleAttr_Xn ) ) == 1 )  
                    hash_key[j] = 1;
                else 
                    hash_key[j] = 0;
            H[hash_key]++;
        }
    }
    else if( model.get( GRB_IntAttr_Status ) == GRB_INFEASIBLE )
    {
        cout << "No terms " << endl;
        exit(-2);
    }
    else
    {
        cout << "Other status " << GRB_IntAttr_Status <<  endl;
        exit(-1);
    }
    
    //collect the u' from which we have odd trails, i.e., the set U
    for ( auto it : H )
        if ( it.second % 2 == 1 )
            Terms.push_back( it.first );
   // cout << "Exact Solutions: "  << Terms.size() << endl;
}
int  MidSolutionCounter(int rounds, const Monomial &start, const Monomial &last, 
						double& time){
	//setting the enviroment
    GRBEnv env = GRBEnv();
    env.set(GRB_IntParam_LogToConsole, 0);
    env.set(GRB_IntParam_Threads, 48);
    env.set(GRB_IntParam_PoolSearchMode, 2);
    env.set(GRB_IntParam_PoolSolutions, MAX);
    
    GRBModel model = GRBModel(env);
    //initialize the variables
	vector<GRBVar> s(STATE);
	for(int i=0; i<STATE; i++){
		s[i] = model.addVar(0,1,0,GRB_BINARY);
	}
	   // set the initial state, note the last three bits are constant 1
    for ( int i = 0; i < STATE; i++ )
		if ( start[i] == 0 )
            model.addConstr( s[i] == 0 );
		else
            model.addConstr( s[i] == 1 );
            
	//propagate the monomials
	vector<GRBVar> works = s;
	for(int r=0; r<rounds; r++){
		Core(model, works, 128, 81, 58, 43, 37, 0);
		vector<GRBVar> temp = works;
		//Value Rotation
        for (int j = 0; j < 128; j++) 
            works[j] = temp[(j+1) % 128];
        //Key Rotation
        //k_128........k_255
        for (int j = 0; j < 128; j++) 
            works[128 + j] = temp[128 + ((j+1) % 128)];  
	}
	
	// the tail is the monomial in U
    for ( int i = 0; i < STATE; i++ )
        if ( last[i] == 1)
            model.addConstr( works[i] == 1 );
        else
            model.addConstr( works[i] == 0 );
            
	model.set(GRB_DoubleParam_TimeLimit, 300.0 );
    model.optimize();
    int solnum = 0;	
    if ( model.get( GRB_IntAttr_Status ) == GRB_TIME_LIMIT ){
	    cout << "Reached Time Limit=>-------------- EXPAND" << endl;
	    vector<Monomial> T;
	    
	    int re = 0;
        do 
	    {
		    T.clear();
            re ++;
            SecondBackExpandPolynomial(re,T,last );
	    }while( T.size() <= 16 && ( re + 10 ) < rounds );
	    int tsize = T.size();
	    int c = 0;
        for ( auto it : T )
        {
            //cout << c << " out of " << tsize << "| Depth " << depth << endl;
            c++;
	        double mytime; 
            solnum += MidSolutionCounter( rounds - re, start, it, mytime );    
        }
	    depth --;
	 }
	 else{
         time = model.get(GRB_DoubleAttr_Runtime );
         int solCount = model.get(GRB_IntAttr_SolCount);
			
         return solCount;
     }
     return solnum;
}


int deg_upbound(int rounds, int backrounds, vector<Monomial> &Terms, 
				Monomial &input, vector<int> Target){
	GRBEnv env = GRBEnv();
    env.set(GRB_IntParam_LogToConsole, 0);
    env.set(GRB_IntParam_Threads, 48 );
    env.set(GRB_IntParam_PoolSolutions, MAX);
    
    GRBModel model = GRBModel(env);
    //initialize the variables
	vector<GRBVar> s(STATE);
	for(int i=0; i<STATE; i++){
		s[i] = model.addVar(0,1,0,GRB_BINARY);
	}
	
	//propagate the monomials
	vector<GRBVar> works = s;
	for(int r=0; r<rounds; r++){
		Core(model, works, 128, 81, 58, 43, 37, 0);
		vector<GRBVar> temp = works;
		//Value Rotation
        for (int j = 0; j < 128; j++) 
            works[j] = temp[(j+1) % 128];
        //Key Rotation
        //k_128........k_255
        for (int j = 0; j < 128; j++) 
            works[128 + j] = temp[128 + ((j+1) % 128)];  
	}
	
	//output constraint
	GRBLinExpr nk = 0;
    for ( int i = 0; i < STATE; i++ )
    {
        if (std::find(Target.begin(), Target.end(), i) != Target.end())
            nk += works[i];
        else 
            model.addConstr( works[i] == 0);
    }
    model.addConstr( nk == 1 );
    
    //Add Condition on Input
    /*
    for ( int i = 58; i < 81; i++ )
    {
        model.addConstr( s[i] == s[i+47]);
    }
    */
    for ( int i = 0; i < 58; i++ )
    {
        model.addConstr( s[i] == 0);
    }
    for ( int i = 81; i < 105; i++ )
    {
        model.addConstr( s[i] == 0);
    }
    
    //input constraint
    GRBLinExpr nv = 0;
    for ( int i = 0; i < 128; i++ )
        nv += s[i];
    // max the degree as the objective function
    model.setObjective(  nv, GRB_MAXIMIZE );
    
    //Number of terms for which we need to find an input monomial
    int size = Terms.size();
    
    while( true ){
    	//first solve, if solution exist then for that solution 
    	//proceed further
    	model.update();
        model.optimize();
        if ( model.get( GRB_IntAttr_Status ) == GRB_OPTIMAL ){
            double time = model.get(GRB_DoubleAttr_Runtime );
    		for ( int j = 0; j < STATE; j++ ) 
                if ( round( s[j].get( GRB_DoubleAttr_Xn ) ) == 1 )  
                    input[j] = 1;
                else 
                    input[j] = 0;
                    
            int solCount = 0;
			// divide-and-conquer
            for (int i=0; i<size; i++ )
            {
                int solnum = MidSolutionCounter(rounds-backrounds,input,Terms[i],time );
                solCount += solnum;
            } 
            
            //confirmed
            if ( solCount % 2 == 1 )
                return round ( model.getObjective().getValue() ); //degree bound
            else  // eliminating the solution
            {
                GRBLinExpr excludeCon = 0;
                for (int i = 0; i < STATE; i++)
                    if (input[i] == 1)
                        excludeCon += (1 - s[i]);
                    else
                        excludeCon += s[i];
                model.addConstr( excludeCon >= 1);
            }   
    	}
    }
}
void BackExpandPolynomial(int rounds, vector<Monomial> &Terms, 
						 vector<int> Target){
	GRBEnv env = GRBEnv();
	env.set(GRB_IntParam_LogToConsole, 0);
	env.set(GRB_IntParam_PoolSearchMode, 2);
	env.set(GRB_IntParam_PoolSolutions, MAX);
	
	GRBModel model = GRBModel( env );
	
	//declare the variables
	vector<GRBVar> s(STATE);
	for(int i=0; i<STATE; i++){
		s[i] = model.addVar(0,1,0,GRB_BINARY);
	} 
	
	//propagate the monomials
	vector<GRBVar> works = s;
	for(int r=0; r<rounds; r++){
		Core(model, works, 128, 81, 58, 43, 37, 0);
		vector<GRBVar> temp = works;
		//Value Rotation
        for (int j = 0; j < 128; j++) 
            works[j] = temp[(j+1) % 128];
        //Key Rotation
        //k_128........k_255
        for (int j = 0; j < 128; j++) 
            works[128 + j] = temp[128 + ((j+1) % 128)];
            
	}
	GRBLinExpr nk = 0;
    for ( int i = 0; i < STATE; i++ )
    {
        if (find(Target.begin(), Target.end(), i) != Target.end())
            nk += works[i];
        else 
            model.addConstr( works[i] == 0);
    }
    model.addConstr( nk == 1 );

    model.update();
    model.write("./data/tinyJambu_inverse_degree.lp");
    model.optimize();
    
    //Prepare the hash table for each solution
    map<Monomial, int, cmp> H;
    if( model.get( GRB_IntAttr_Status ) == GRB_OPTIMAL){
    	double time = model.get(GRB_DoubleAttr_Runtime );
        cout << "Time Used: " << time << "sec" << endl;
        
        int solCount = model.get(GRB_IntAttr_SolCount);
        cout << "Raw Solutions: " << solCount << endl;
       
        bitset<STATE> hash_key;
        for(int i=0; i<solCount; i++){
        	model.set(GRB_IntParam_SolutionNumber, i );
        	for ( int j = 0; j < STATE; j++ ) 
                if ( round( s[j].get( GRB_DoubleAttr_Xn ) ) == 1 )  
                    hash_key[j] = 1;
                else 
                    hash_key[j] = 0;
            H[hash_key]++;
        }
    }
    else if( model.get( GRB_IntAttr_Status ) == GRB_INFEASIBLE )
    {
        cout << "No terms " << endl;
        exit(-2);
    }
    else
    {
        cout << "Other status " << GRB_IntAttr_Status <<  endl;
        exit(-1);
    }
    
    //collect the u' from which we have odd trails, i.e., the set U
    for ( auto it : H )
        if ( it.second % 2 == 1 )
            Terms.push_back( it.first );
    cout << "Exact Solutions: "  << Terms.size() << endl;
}
int main(){
	DV DEG[900];
	int MID = 1; //First expand in backward up to MID 
	int str = 2, end = 736;
	vector<int > Target = {127};
	vector<Monomial> Terms;
	BackExpandPolynomial(MID, Terms,Target);
	cout << "Printing monomials presents after " << MID << " rounds"<<endl;
    cout << "Target "<<Target[0]<<endl;
    cout<<Terms[0]<<endl;
    PRINT_VEC(Terms);
	
	
	FILE *fpdeg;
	fpdeg = fopen("./data/tinyjambu_inverse_cube_105_127.txt","w");
	//FIND degree of each round
    for (int i=str; i<end; i++ )
    {
        cout << "++++++++++++++++++++++++++++++++++" << endl;  
        cout << "Round: " << i << endl;
        cout << getCurrentSystemTime() << endl;
        DEG[i].deg = deg_upbound( i, MID, Terms, DEG[i].vec, Target );
        cout <<"-----------------------------------" << endl;
        cout << "Round: " << i << " | "  << "Upbound: " <<  DEG[i].deg << endl;
        fprintf(fpdeg,"Round: %d => Deg_upbound: %d Monomial: ",i,DEG[i].deg); 
	    for ( int j = 0; j < 128; j++ )
            if ( DEG[i].vec[j] == 1 )
            	fprintf(fpdeg,"v%04d",j); 
        cout << getCurrentSystemTime() << endl;
        fprintf(fpdeg,"\n"); 
        fflush(fpdeg);
    }
    fclose(fpdeg);
}
