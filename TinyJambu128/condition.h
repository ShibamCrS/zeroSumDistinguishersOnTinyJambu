void condition(GRBModel &model, vector<GRBVar>& s, int con_no){
	if (con_no == 0){
		for ( int i = 23; i < 128; i++ ){
        model.addConstr( s[i] == 0);
    	}
	}
	else if (con_no == 1){
		for ( int i = 0; i < 96; i++ ){
        model.addConstr( s[i] == 0);
    	}
	}
	else if (con_no == 2){
		for ( int i = 0; i < 58; i++ ){
        model.addConstr( s[i] == 0);
    	}
    	for ( int i = 81; i < 105; i++ ){
        model.addConstr( s[i] == 0);
    	}
	}
	else if (con_no == 3){
		for ( int i = 0; i < 32; i++ ){
        model.addConstr( s[i] == 0);
    	}
	}
	else if (con_no == 4){
		for ( int i = 23; i < 80; i++ ){
        model.addConstr( s[i] == 0);
    	}
	}

}
void key_condition(GRBModel &model, vector<GRBVar>& s, int con_no){
	if (con_no == 0){
		for ( int i = 23; i < 128; i++ ){
        model.addConstr( s[i] == 0);
    	}
	}
	else if (con_no == 1){
		for ( int i = 0; i < 96; i++ ){
        model.addConstr( s[i] == 0);
    	}
	}
	else if (con_no == 2){
		for ( int i = 0; i < 58; i++ ){
        model.addConstr( s[i] == 0);
    	}
    	for ( int i = 81; i < 105; i++ ){
        model.addConstr( s[i] == 0);
    	}
	}
	else if (con_no == 3){
		for ( int i = 0; i < 32; i++ ){
        model.addConstr( s[i] == 0);
    	}
	}

}
