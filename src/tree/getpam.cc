// Hacky way of getting the PAM spectral decomposition from an integer score matrix.
// This causes lots of rounding errors...
// Some day I may do PAM properly, from the original Dayhoff mutation data.
// Until then, this code is really not meant for general use (it generates source code for 'pam.cc')

#include <math.h>
#include "newmat/newmat.h"
#include "newmat/newmatap.h"
#include "newmat/newmatio.h"
#include "util/logfile.h"

// Dayhoff frequencies
// Amino acid order: arndcqeghilkmfpstwyv
static double fq[20] = {
87.13, 40.90, 40.43, 46.87, 33.47, 38.26, 49.53, 88.61, 33.62, 36.89,
85.36, 80.48, 14.75, 39.77, 50.68, 69.58, 58.54, 10.49, 29.92, 64.72};

int main (int argc, char** argv)
{
  try
    {
      if (argc != 3) THROWEXPR ("Usage: " << argv[0] << " <PAM matrix> <time>");

      double tot = 0;
      for (int i = 0; i < 20; ++i) tot += fq[i];
      for (int i = 0; i < 20; ++i) fq[i] /= tot;

      Matrix P (20, 20);

      double t = atof (argv[2]);
      ifstream pam_s (argv[1]);

      double P_tot = 0;
      while (pam_s && !pam_s.eof())
	{
	  sstring s;
	  for (int i = 0; i < 20; ++i)
	    {
	      s.getline (pam_s);
	      s.chop();
	      vector<sstring> f = s.split();
	      if (f.size() != 20) THROWEXPR ("Need 20 fields per line; line '" << s << "' has " << f.size());
	      for (int j = 0; j < 20; ++j)
		{
		  P(i+1,j+1) = pow (2, f[j].to_double()/2) * fq[i] * fq[j];
		  P_tot += P(i+1,j+1);
		}
	    }
	}
      //      cout << P << "\n";
      //      cout << "P_tot = " << P_tot << "\n";

      for (int i = 0; i < 20; ++i)
	for (int j = 0; j < 20; ++j)
	  P(i+1,j+1) /= P_tot;

      cerr << "P totals:\n";
      for (int i = 0; i < 20; ++i)
	{
	  double row_tot = 0;
	  for (int j = 0; j < 20; ++j)
 	    row_tot += P(i+1,j+1);
	  cerr << "i=" << i << " fq[i]=" << fq[i] << " row_tot(i)=" << row_tot << "\n";
	}

      DiagonalMatrix inv_sqrt_freq_diag (20);
      for (int i = 0; i < 20; ++i) inv_sqrt_freq_diag(i+1,i+1) = 1.0 / sqrt(fq[i]);
      SymmetricMatrix eSt (20);
      eSt << inv_sqrt_freq_diag * P * inv_sqrt_freq_diag;    // have to use <<, not =, to prevent a newmat exception

      DiagonalMatrix eDt;
      Matrix V;
      Jacobi_catch (eSt, eDt, V);

      //      cout << "eSt - decomposition:\n" << eSt - V * eDt * V.t() << "\n";

      cerr << "P - decomposition:\n" << P - inv_sqrt_freq_diag.i() * V * eDt * V.t() * inv_sqrt_freq_diag.i() << "\n";

      cout << "// Autogenerated by " << __FILE__ << "\n// Do not edit this file directly\n\n#include \"pam.h\"\n\nconst Alphabet& PAM_factory::alphabet()\n{\n  return Protein_alphabet;\n}\n\n";
      
      cout << "PAM_factory::PAM_factory()\n{\n  const int alph_sz = 20;\n\n  const double eval[] = { ";
      for (int i = 0; i < 20; ++i) { cout << min (0.0, log(eDt(i+1,i+1)) / t); if (i < 19) cout << ", "; }
      cout << " };\n  eigenvalue = vector<double> (eval, eval + alph_sz);\n";
      
      for (int i = 0; i < 20; ++i)
	{
	  cout << "\n  const double evec" << i << "[] = { ";
	  for (int j = 0; j < 20; ++j) { cout << V(j+1,i+1); if (j < 19) cout << ", "; }
	  cout << " };\n  eigenvector.push_back (vector<double> (evec" << i << ", evec" << i << " + alph_sz));\n";
	}
      
      cout << "}\n";
    }
  catch (const Dart_exception& e)
    {
      CLOGERR << e.what();
      exit(1);
    }
}