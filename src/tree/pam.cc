// Autogenerated by /home/yam/palace/thmm/getpam.cc
// Do not edit this file directly

#include "tree/pam.h"

const Alphabet& PAM_factory::alphabet()
{
  return Protein_alphabet;
}

PAM_factory::PAM_factory()
{
  const int alph_sz = 20;

  const double eval[] = { -0.237688, -0.572589, -2.87135, -1.80086, 0, -1.50997, -2.00378, -0.785944, -1.13116, -1.72489, -0.290841, -0.84021, -1.22413, -0.683149, -0.638375, -1.38183, -1.0694, -0.407039, -0.371537, -0.949449 };
  eigenvalue = vector<double> (eval, eval + alph_sz);

  const double evec0[] = { 0.152773, 0.108573, 0.102903, 0.133593, -0.0521415, 0.0956828, 0.138341, 0.177849, 0.0537749, -0.0502762, -0.239453, 0.147873, -0.0399386, -0.580187, 0.180554, 0.127032, 0.10278, -0.0744991, -0.62137, -0.0025391 };
  eigenvector.push_back (vector<double> (evec0, evec0 + alph_sz));

  const double evec1[] = { -0.256755, 0.436768, 0.125584, 0.118759, 0.0513453, 0.213852, 0.111448, -0.285538, 0.230064, -0.0301433, 0.0318147, 0.446052, 0.0521453, -0.0761061, -0.515726, -0.0865151, -0.0915485, -0.129092, 0.0483333, -0.0891585 };
  eigenvector.push_back (vector<double> (evec1, evec1 + alph_sz));

  const double evec2[] = { -0.0375089, 0.0157756, 0.387055, -0.708228, 0.00360566, -0.089634, 0.573679, 0.00639143, -0.041365, -0.0135321, 0.000388336, -0.0434103, 0.00427688, 0.00223345, 0.00209585, -0.0761202, 0.00934563, -0.000659095, -0.00784392, 0.00107364 };
  eigenvector.push_back (vector<double> (evec2, evec2 + alph_sz));

  const double evec3[] = { 0.00856931, -0.0444615, 0.248271, -0.0996798, -0.0277668, 0.244006, -0.165036, -0.0906408, -0.170996, 0.219306, -0.0282009, -0.0719786, 0.0314602, -0.0179961, -0.0385066, 0.662981, -0.547551, -0.00683302, 0.00210723, -0.0714027 };
  eigenvector.push_back (vector<double> (evec3, evec3 + alph_sz));

  const double evec4[] = { 0.31025, 0.193263, 0.205601, 0.201325, 0.163302, 0.183367, 0.215991, 0.267732, 0.175526, 0.1737, 0.338652, 0.255342, 0.134055, 0.218463, 0.262993, 0.263987, 0.245453, 0.0672774, 0.207424, 0.227492 };
  eigenvector.push_back (vector<double> (evec4, evec4 + alph_sz));

  const double evec5[] = { 0.724318, -0.019642, 0.00497611, -0.0227006, -0.00334404, -0.259797, -0.0161941, -0.22691, 0.165461, -0.301918, 0.0194674, 0.130948, -0.0166728, 0.0187262, -0.104149, -0.119892, -0.406036, 0.000964432, -0.014422, 0.167925 };
  eigenvector.push_back (vector<double> (evec5, evec5 + alph_sz));

  const double evec6[] = { -0.175456, -0.043419, -0.651771, -0.0472714, -0.015177, -0.291039, 0.412651, -0.000146395, 0.245448, -0.0155583, 0.00999212, 0.117788, -0.00933149, -0.00676297, -0.00121627, 0.443471, -0.107621, -0.00257243, 0.00568227, 0.0468215 };
  eigenvector.push_back (vector<double> (evec6, evec6 + alph_sz));

  const double evec7[] = { 0.117015, 0.350693, -0.169256, -0.381848, -0.0468014, -0.247171, -0.383005, 0.0969884, -0.301696, 0.225844, -0.203347, 0.261105, 0.103521, -0.0759263, -0.147824, 0.117676, 0.23901, -0.0382571, 0.0725524, 0.319214 };
  eigenvector.push_back (vector<double> (evec7, evec7 + alph_sz));

  const double evec8[] = { -0.171856, -0.567062, 0.0679906, -0.202255, 0.00971191, 0.0734455, -0.234357, 0.172949, 0.33803, 0.0630586, -0.113004, 0.515657, 0.26578, -0.00636973, 0.0897973, -0.127422, -0.113056, 0.0448448, -0.0108214, 0.0790687 };
  eigenvector.push_back (vector<double> (evec8, evec8 + alph_sz));

  const double evec9[] = { 0.318545, -0.0134678, -0.126687, -0.0238445, 0.00292025, -0.0967741, 0.0678765, -0.0416801, 0.107078, 0.688129, -0.030415, 0.0401257, 0.0204408, -0.0347425, -0.032172, -0.171906, 0.00774146, 0.00300406, 0.00271163, -0.588388 };
  eigenvector.push_back (vector<double> (evec9, evec9 + alph_sz));

  const double evec10[] = { 0.00184398, 0.0320688, 0.0181647, 0.024284, -0.957418, 0.0381674, 0.029567, 0.00226549, 0.0193655, 0.043477, 0.238593, 0.0512065, 0.0701549, 0.0713339, 0.00353296, -0.0281654, 0.0112923, 0.0436926, -0.0532446, 0.0395073 };
  eigenvector.push_back (vector<double> (evec10, evec10 + alph_sz));

  const double evec11[] = { 0.0194596, -0.251938, 0.0883342, 0.144281, -0.0287224, 0.0592824, 0.148453, -0.611316, 0.0954274, 0.352054, -0.263602, -0.13204, 0.00184463, -0.0107077, -0.0268702, 0.039774, 0.249713, 0.0368616, -0.0288669, 0.469579 };
  eigenvector.push_back (vector<double> (evec11, evec11 + alph_sz));

  const double evec12[] = { 0.0774295, 0.0792606, -0.0638524, 0.0362701, 0.00194187, 0.0537588, 0.0736056, -0.0574548, -0.0200858, -0.175759, -0.177088, -0.241323, 0.907889, 0.0087887, -0.0288666, 0.0375967, 0.0646001, -0.00307011, 0.0058187, -0.137923 };
  eigenvector.push_back (vector<double> (evec12, evec12 + alph_sz));

  const double evec13[] = { -0.0170349, 0.0897096, -0.00625436, -0.000916056, 0.0465075, -0.000931081, -0.00524553, 0.0292486, -0.0197301, 0.0249912, -0.18476, 0.0758092, -0.0244825, 0.754831, -0.0451495, 0.0138112, -0.00585027, -0.0395475, -0.610739, -0.0219085 };
  eigenvector.push_back (vector<double> (evec13, evec13 + alph_sz));

  const double evec14[] = { -0.125773, 0.326812, -0.0963977, -0.192652, 0.0149767, 0.0632277, -0.172063, -0.451653, 0.111099, -0.0559634, 0.0434454, 0.111374, 0.0113963, 0.0185792, 0.729123, -0.0815551, -0.0926548, -0.0730935, 0.00457254, -0.103384 };
  eigenvector.push_back (vector<double> (evec14, evec14 + alph_sz));

  const double evec15[] = { -0.246926, 0.13532, 0.439154, 0.219234, -0.00882055, -0.728919, -0.133362, 0.0517164, 0.280947, 0.102177, 0.00160188, -0.117874, 0.0851822, -0.0160518, 0.0598926, 0.0969465, -0.0603758, -0.0131564, -0.0178706, -0.00555479 };
  eigenvector.push_back (vector<double> (evec15, evec15 + alph_sz));

  const double evec16[] = { 0.146243, -0.146307, 0.0884023, -0.225878, -0.022172, 0.00499251, -0.264699, -0.216071, 0.294503, -0.2894, 0.119003, -0.0348962, -0.12354, 0.0228871, -0.1584, 0.394014, 0.529768, -0.00516348, -0.036199, -0.342235 };
  eigenvector.push_back (vector<double> (evec16, evec16 + alph_sz));

  const double evec17[] = { -0.0534549, 0.125311, -0.0174761, -0.0314216, 0.0567915, 0.0024949, -0.0324329, -0.065885, 0.0073961, -0.0134403, 0.0316061, 0.0296611, 0.0056839, -0.0592443, -0.043891, -0.0158695, -0.0306948, 0.973807, -0.117524, -0.0252 };
  eigenvector.push_back (vector<double> (evec17, evec17 + alph_sz));

  const double evec18[] = { 0.0688182, 0.0966927, 0.100992, 0.115989, -0.201896, 0.0559269, 0.112196, 0.13259, 0.0843836, -0.160677, -0.745329, 0.0970068, -0.194853, 0.148373, 0.120214, 0.0795125, 0.0261912, 0.102737, 0.412444, -0.16803 };
  eigenvector.push_back (vector<double> (evec18, evec18 + alph_sz));

  const double evec19[] = { 0.0210312, 0.245898, -0.114241, -0.229342, -0.0153738, 0.263673, -0.17095, 0.247419, 0.61854, 0.132449, -0.0785555, -0.467279, -0.0642943, -0.0206306, -0.0932377, -0.0939369, -0.115283, -0.0219703, -0.0238274, 0.228408 };
  eigenvector.push_back (vector<double> (evec19, evec19 + alph_sz));
}