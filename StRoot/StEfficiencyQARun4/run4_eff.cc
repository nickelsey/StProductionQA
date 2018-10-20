#include "run4_eff.h"


Run4Eff::Run4Eff() :
    parset0({ 0.732881, -0.106894, 0.513479, -0.707841, 0.0629901, -15.3324, -22.0429, 0.0711034, -0.00933597 }),
    parset1({ 0.759782, -0.02679, 0.0902155, 0.0233054, -0.312277, -16.6314, -17.3444, 0.0413476, -0.00442302, 2.95431e+15, 0.564756, 13.5884, -0.576843, 0.0146661 }),
    parset2({ 0.739995, -0.00931128, 0.123802, -0.135568, -0.182505, -1.91149, -7.96689, 0.0422089, -0.00459387, 0.0921504, 0.30705, 0.157101, 0.333912, 0.0258943 }),
    parset3({ 0.753189, 0.0386343, 0.0746096, -0.159341, -0.174597, -1.31007, -5.75126, 0.0223444, -0.00167887, 0.00440079, 0.0647485, 0.00756371, 0.475831, 2.30303 }),
    parset4({ 0.698809, 0.0347652, -0.00819333, 0.112736, -0.356907, -1.62506, -7.26695, 0.0436162, -0.00453185, 0.249514, 0.308879, 0.133046, 0.295414, 0.0019349 }),
    parset5({ 0.652242, 0.0760859, -0.0784171, 0.0393619, -0.247293, -2.04786, -8.96039, 0.0603416, -0.006971, -0.0435101, 0.131005, 0.00053132, 0.74369, 0.00576589 }),
    parset6({ 0.631911, 0.117639, -0.29002, 0.522928, -0.569609, -1.3921, -6.73044, 0.0588101, -0.00686795, 0.110982, 0.2951, 0.14493, 0.295612, 0.00290843 }),
    maxPt(5.0) {

        // initialize y4 AuAu
        for (int i = 0; i < 9; ++i) {
            std::string name = "y4_eff_bin_" + std::to_string(i);
            if (i == 0) {
                y4_functions[i] = new TF2(name.c_str(), "[0]+[1]*x^2+[2]*x^4+[3]*x^6+[4]*x^8+[5]*exp([6]*y)+[7]*y+[8]*y*y",-1.,1.,0.,maxPt);
            }
            else {
                y4_functions[i] = new TF2(name.c_str(),"[0]+[1]*x^2+[2]*x^4+[3]*x^6+[4]*x^8+[5]*exp([6]*y)+[7]*y+[8]*y*y +  [9]*exp(-((abs(x)-[10])^2)  /[11] - ((  abs(y)-[12]  )^2)  /[13]) ",-1.,1.,0.,maxPt);
            }
        }

        ((TF2*)y4_functions[0])->SetParameters(&(parset0[0]));
        ((TF2*)y4_functions[1])->SetParameters(&(parset0[0]));
        ((TF2*)y4_functions[2])->SetParameters(&(parset1[0]));
        ((TF2*)y4_functions[3])->SetParameters(&(parset1[0]));
        ((TF2*)y4_functions[4])->SetParameters(&(parset2[0]));
        ((TF2*)y4_functions[5])->SetParameters(&(parset3[0]));
        ((TF2*)y4_functions[6])->SetParameters(&(parset4[0]));
        ((TF2*)y4_functions[7])->SetParameters(&(parset5[0]));
        ((TF2*)y4_functions[8])->SetParameters(&(parset6[0]));

    }

Run4Eff::~Run4Eff() {
    for (int i = 0; i < 9; ++i)
        delete y4_functions[i];
}

double Run4Eff::AuAuEff(double pt, double eta, int cent) {
    int alt_cent = 8 - cent;
    if (alt_cent < 0 || alt_cent > 8) {
        std::cerr << "warning: centrality class doesn't make sense. Input: " << cent << std::endl;
        std::cerr << "centrality defined for 0-8" << std::endl;
        return 0;
    }
    if (pt > maxPt)
        pt = maxPt;
    return y4_functions[alt_cent]->Eval(eta,pt);
}



