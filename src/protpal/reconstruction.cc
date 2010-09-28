#include<iostream>
#include<string>
#include<fstream>
#include<sstream>

#include "protpal/utils.h"
#include "protpal/profile.h"
#include "protpal/reconstruction.h"
#include "protpal/transducer.h"
#include "tree/phylogeny.h"
#include "util/piper.h"
#include "ecfg/ecfgsexpr.h"
#include "util/unixenv.h"
#include "util/sstring.h"
#include "util/opts_list.h"
#include "seq/alignment.h"
#include "seq/biosequence.h"


#define DEFAULT_CHAIN_FILE "data/handalign/prot3.hsm"
#define DEFAULT_GRAMMAR_FILE "grammars/prot3.eg"
#define DEFAULT_GAP_GRAMMAR_FILE "grammars/prot3gap.eg"

using namespace std; 

Reconstruction::Reconstruction(int argc, char* argv[])
{
  INIT_OPTS_LIST (opts, argc, argv, 0, "[options]",
		  "Align and reconstruct ancestral sequences\n");
  sstring default_chain_filename, default_gap_grammar_filename, default_grammar_filename;
  default_chain_filename << Dart_Unix::get_DARTDIR() << '/' << DEFAULT_CHAIN_FILE;
  default_grammar_filename << Dart_Unix::get_DARTDIR() << '/' << DEFAULT_GRAMMAR_FILE;
  default_gap_grammar_filename << Dart_Unix::get_DARTDIR() << '/' << DEFAULT_GAP_GRAMMAR_FILE;

  have_tree=false; 
  have_sequences=false; 
  bool rndtime = true; 

  opts.program_name = "ProtPal";
  opts.short_description = "Progressive alignment and reconstruction of proteins";
  opts.syntax = "-<format> [SEQUENCE FILE] [OPTIONS] > [RECONSTRUCTION FILE]";
  opts.expect_args = -1; 

  opts.add("l log-level", loggingLevel=1, "<int> Show log messages of varying levels of verbosity. (default = level 1 )");
  opts.print("\tLevel  0 : Show no log messages.  The alignment is sent to standard out.\n");
  opts.print("\tLevel  1 : Top-level logging related to profile creating/sampling, etc.  This is default\n");
  opts.print("\tLevel  2 : Detailed logging including DP, traceback, etc (dense)\n");
  opts.print("\tLevel  3 : Very detailed logging including transducer composition algorithms. (dense)\n");

  opts.newline(); 
  opts.print_title("Input/output options");
  opts.add ("stk -stockholm-file",  stkFileName="None", "Unaligned stockholm sequence file.  If there is a #=GF NH line, this will be used as the phylogenetic tree, though this can be overridden by the -t option.", false);
  opts.add("fa -fasta-file", fastaFileName="None", "Unaligned FASTA sequence file",false );
  opts.add("t -tree-string", treeString="None", "Tree string in newick format, within double quotes. ", false);
  opts.add("tf -tree-file", treeFileName="None", "File containing tree string in newick format.", false);
  opts.add("xo -xrate-output", xrate_output=false, "Display final alignment in  full XRATE-style (will be used if the -anrec-postprob option is called).  Default is a compact Stockholm form. ");
  opts.add("fo -fasta-output", fasta_output=false, "Display final alignment in FASTA format");

  opts.newline(); 
  opts.print_title("Reconstruction options");

  opts.add("g -grammar-file", grammar_filename = default_grammar_filename, "<grammar filename> DART format grammar to be used for final character reconstruction");
  opts.add("a -leaves-only", leaves_only = false, "Show only leaves when displaying alignments, e.g. don't to ancestral reconstruction: 'alignment' mode (default false) ");


  opts.add("arpp -ancrec-postprob", ancrec_postprob = false,"report posterior probabilities of alternate reconstructions");
  opts.add("marp -min-ancrec-prob", min_ancrec_postprob =0.01,   "minimum probability to report for --ancrec-postprob option");
  opts.add("tr -stochastic-traceback", stoch_trace=false,   "Stochastically trace through null states, rather than the most probable (Viterbi) path through transducers (ML alignment)");
  

  opts.newline(); 
  opts.print_title("Simulation Options"); 

  opts.add("s -simulate", simulate=false,"simulate a full (ancestors + leaves) set of aligned sequences according to the models (e.g. transducers, rate matrix, tree) specified.");
  opts.add("sa -show-alignments", show_alignments=false, "show intermediate sampled alignments  ");
  opts.add("rl -root-length", rootLength=-1, "<int> Root sequence length in simulation.  Default is to sample direclty from singlet transducer's insert distribution.", false);

  opts.newline();
  opts.print_title("Model parameters");

  opts.add("b -subst-model", rate_matrix_filename = default_chain_filename, "<rate matrix file> DART format chain file to be used for branch transducers' match states absorb/emit likelihoods.");
  opts.add("bs -subst-scale", sub_rate = 1.0, "Substitution rate scaling parameter ");
  opts.add("i -insert-rate", ins_rate=0.0025,"Insertion rate ");
  opts.add("d -delete-rate", del_rate=0.0025,"Deletion rate ");
  opts.add("ri -root-insert-prob", root_insert_prob=0.999, "Insert probability at root"); 
  opts.add("eri -estimate-root-insert-prob", estimate_root_insert=false, "Estimate insertion probability at root by averaging leaf sequence length");
  opts.add("x -gap-extend", gap_extend=0.9, "Gap extend probability");
  opts.add("n -num-samples", num_sampled_paths=100, "Number of paths to sample in traceback");
  opts.add("m -max-DAG-size", max_sampled_externals=1000, "Max number (approximately) of delete states allowed in DAG");
  opts.add("e -max-distance", envelope_distance=300, "Maximum allowed distance between aligned leaf characters");

  opts.newline();
  opts.print_title("Indel rate investigation");
  opts.add("pi -print-indels", indel_filename = "None", "Show inserted and deleted sequences, as well as estimated indel open and extend rates - written to specified file.");
  opts.add("db -stk-db", db_filename = "None", "Show alignments sampled at root level as a stockholm database - written to specified file.");
  opts.add("ra -root-alignments", num_root_alignments=1, "Number of alignments to sample at root node.");
  opts.add("ep -estimate-parameters", estimate_params=false, "Estimate the indel rate parameters via a stochastic sample (default 100 alignments, unless set by -ra option) at the root level\n");
  opts.add("ia -input-alignment", input_alignment=false, "Estimate indel rate parameters via a fixed input alignment (specified as -stk or -fa), rather than using protpal's internal alignment/reconstruction algorithm.");
  opts.add("gc -gap-chain", gap_grammar_filename = default_gap_grammar_filename, "<grammar_filename> use this dart grammar containing a rate matrix with a 'gap' character.  This is used when reconstructing with fixed input alignment");
  opts.add("tg -train-grammar", train_grammar = false, "Use EM algorithm to estimate the character substitution model's parameters before inferring ancestral characters.  This is especially useful when using a fixed alignment, since the 'gap chain' is essentially part of the indel model");

  

  opts.parse_or_die(); 
  string error=""; bool all_reqd_args=true; 
  if (!estimate_params && num_root_alignments != 1)
    estimate_params = true; 
  viterbi = !stoch_trace;

  // Make sure we have the essential data - sequences and a tree
  // First, make sure we have sequence data from somewhere
  if(stkFileName == "None" && fastaFileName =="None" && !simulate)
	{
	  error += "\tNo sequence file could be imported.  Use -stk or -fa  to specify a sequence file\n";
	  all_reqd_args = false; 
	}
  // Next, see if we have a tree, first trying to get it from a #=GF NH stockholm line
  if (stkFileName != "None")
    get_stockholm_tree(stkFileName.c_str());
  if (treeString == "None" && treeFileName == "None" && !have_tree )
	{
	  error += "\tNo tree string was specified.  Use -t  or -tf <to specify a phylogenetic tree, or include it the stockholm file as a  '#=GF NH' line \n";
	  all_reqd_args = false; 
	}
  else
    {
      if (treeString != "None")
	loadTreeString(treeString.c_str());
      else if (treeFileName != "None")
	get_tree_from_file(treeFileName.c_str());
    }
  if(!all_reqd_args)
    {
      std::cout<<"\nNot all required arguments were supplied:\n"<<error<<endl;  
      std::cout<<opts.short_help(); 
      exit(1);
    }
  // Otherwise, do some preliminary stuff
  //seed rand on the clock time          
  if (rndtime)
    srand((unsigned)time(0));
  // Name internal nodes of the tree, if not already named
  set_node_names(); 

}


void Reconstruction::parse_sequences(Alphabet alphabet)
{
  // stockholm or fasta? (maybe add more later)
  if (stkFileName != "None")
    sequences = parse_stockholm(stkFileName.c_str(), alphabet); 
  else if (fastaFileName != "None")
    sequences = parse_fasta(fastaFileName.c_str(), alphabet); 
}

  
void Reconstruction::loadTreeString(const char* in)
{
  if (loggingLevel>=2) std::cerr<<"Loading tree string: "<<in<<endl; 
  const sstring tree_string = in; 
  istringstream tree_input (tree_string);
  PHYLIP_tree in_tree;
  in_tree.read (tree_input);
  tree = in_tree; 
}

void Reconstruction::set_node_names(void)
{
  for (int node=0; node<tree.nodes(); node++)
    {
      if (tree.node_name[node].size() == 0)
	{
	  if (loggingLevel >= 2)
	    std::cerr<<"Naming node "<< node << endl; 
	  stringstream out;
	  out << "Node_"<<node;
	  tree.node_name[node] = sstring(out.str()); 
	}
    }
}


void Reconstruction::get_tree_from_file(const char* fileName)
{
  string line;
  ifstream treeFile(fileName);
  string tree_tmp = ""; 
  if (treeFile.is_open())
    {
      while (! treeFile.eof() )
        {
	  getline(treeFile,line);
	  if (index(";", line) != -1)
	    {
	      const char* tree = line.c_str(); 
	      loadTreeString(tree); 
	    }
	}
    }

}

void Reconstruction::get_stockholm_tree(const char* fileName)
{
  string line;
  ifstream seqFile(fileName);
  string tree_tmp = ""; 
  const char* tree_string; 
  vector<string> splitLine; 
  // Parse stockholm file                                                                                                    
  if (seqFile.is_open())
    {
      while (! seqFile.eof() )
        {
		  getline(seqFile,line);
		  splitLine = split(line, " "); 
		  //std::cerr<<"Line: "<<line<<endl;
          if (splitLine.size()<2) continue;
		  else if (splitLine[1] == "NH")
            {
			  for (unsigned int i= 2; i<splitLine.size(); i++) // newick string possibly has spaces!
				tree_tmp += splitLine[i]; 

			  tree_string = tree_tmp.c_str(); 
			  treeString = tree_string; // Silly I know. 
			  loadTreeString(tree_string); 
			  have_tree = true; 
            }
		}
      seqFile.close();
    }
  else
    {
      std::cerr << "Error: Unable to open file: "<<fileName << endl; 
      exit(1); 
    }
}

  
void Reconstruction::make_sexpr_file(Alphabet alphabet, Irrev_EM_matrix rate_matrix)
{
  map<string,string> prot2pc;
  prot2pc["S"] = "start";
  prot2pc["M"] = "match"; 
  prot2pc["I"] = "insert"; 
  prot2pc["D"] = "delete";
  prot2pc["W"] = "wait";
  prot2pc["E"] = "end";   
  
  std::cout << ";; phylocomposer file for simulating branch-length dependent transducers\n";
  std::cout << "(token ("  ; 
  vector<sstring> tokens = alphabet.tokens(); 
  unsigned int alph_size = tokens.size();
  vector<sstring>::iterator tok, tok2; 
  unsigned int tokIdx, tokIdx2; 
  vector<state>::iterator state1, state2; 
  vector<state> outgoing; 
  string parent, child; 

  for (tok = tokens.begin(); tok != tokens.end(); tok++)
	std::cout << " "<< *tok << " "; 
  std::cout<< ") )\n"; 

  // define pi, the shared equilibrium distribution over tokens
  vector<double> equilibrium = rate_matrix.create_prior();
  for (tokIdx = 0 ; tokIdx != alph_size; tokIdx++)
	std::cout << "(value ((pi " << tokens[tokIdx] << ") " << equilibrium[tokIdx] << "))\n";
  
  
  // Define parameters for, then declare singlet transducer
  SingletTrans R(alphabet, rate_matrix);
  
  // Define parameters in 'value' blocks
  std::cout<<"\n";
  for (state1 = R.states.begin(); state1 != R.states.end(); state1++)
	{
	  if (R.get_state_type(*state1) == "E") continue; 
	  outgoing = R.get_outgoing(*state1); 
	  for (state2 = outgoing.begin(); state2 != outgoing.end(); state2++)
		{
		  std::cout<< "(value (root_"<<R.get_state_name(*state1)<<"_"<<R.get_state_name(*state2)<<" ";
		  std::cout<< R.get_transition_weight(*state1, *state2)<<"))\n";
		}
	} 

  std::cout<<"\n(transducer \n\n\t(name ROOT)\n\n"; 
  
  for (state1 = R.states.begin(); state1 != R.states.end(); state1++)
	{
	  std::cout<<"\t(state (name "<< R.get_state_name(*state1)<< ") (type "<< prot2pc[R.get_state_type(*state1)] << ")";
	  if (R.get_state_type(*state1) == "I")
		std::cout<<" (label pi)"; 
	  std::cout<<")\n";
	}

  // Define transitions, referring back to earlier specified values
  std::cout<<"\n";
  for (state1 = R.states.begin(); state1 != R.states.end(); state1++)
	{
	  if (R.get_state_type(*state1) == "E") continue; 
	  outgoing = R.get_outgoing(*state1); 
	  for (state2 = outgoing.begin(); state2 != outgoing.end(); state2++)
		{
		  std::cout<<"\t(transition (from "<< R.get_state_name(*state1) << ") ";
		  std::cout<<"(to "<< R.get_state_name(*state2) << ") ";
		  std::cout<< "(label root_"<<R.get_state_name(*state1)<<"_"<<R.get_state_name(*state2)<<" ";
		  std::cout<<"))\n";
		}
	} 
  std::cout<<");; end transducer ROOT\n\n";



  for_nodes_post (tree, tree.root, -1, bi)
    {
      const Phylogeny::Branch& b = *bi;
      node treeNode = b.second;
	  if (treeNode != tree.root)
		{
		  parent = string( tree.node_name[b.first].c_str() );
		  child = string( tree.node_name[b.second].c_str() ); 	  

		  // construct a branch transducer with the appropriate branch length.
		  BranchTrans branch(tree.branch_length(b.first, b.second), alphabet, rate_matrix, ins_rate, del_rate, gap_extend, sub_rate); 
		  
		  // define the Q_child match matrix
		  for (state1 = branch.states.begin(); state1 != branch.states.end(); state1++)
			{
			  if ( branch.get_state_type(*state1) != "M" ) 
				continue;
			  for (tokIdx = 0 ; tokIdx != alph_size; tokIdx++)
				{
				  for (tokIdx2 = 0 ; tokIdx2 != alph_size; tokIdx2++)
					{
					  std::cout<< "(value ((Q_"<< child<< " " << tokens[tokIdx] << " "<< tokens[tokIdx2] << ") "; 
					  std::cout<<  branch.get_match_weight(*state1, tokIdx, tokIdx2);
					  std::cout<< "))\n";
					}
				}
			}
		  

		  // Define transition parameters in 'value' blocks
		  std::cout<<"\n";
		  for (state1 = branch.states.begin(); state1 != branch.states.end(); state1++)
			{
			  if (branch.get_state_type(*state1) == "E") continue; 
			  outgoing = branch.get_outgoing(*state1); 
			  for (state2 = outgoing.begin(); state2 != outgoing.end(); state2++)
				{
				  std::cout<< "(value ("<< child <<"_"<<branch.get_state_name(*state1)<<"_"<<branch.get_state_name(*state2)<<" ";
				  std::cout<< branch.get_transition_weight(*state1, *state2)<<"))\n";
				}
			} 


		  // begin transducer declaration
		  std::cout<<"\n(transducer \n\n\t(name "<<child<< ")\n\n"; 

  		  //  declare states, possibly with labels for insert and match states
		  for (state1 = branch.states.begin(); state1 != branch.states.end(); state1++)
			{
			  std::cout<<"\t(state (name "<< branch.get_state_name(*state1)<< ") (type "<< prot2pc[branch.get_state_type(*state1)] << ")";
			  if (branch.get_state_type(*state1) == "I")
				std::cout<<" (label pi)"; 
			  else if  (branch.get_state_type(*state1) == "M")
				std::cout<<" (label Q_"<<child<<")"; 
					 
			  std::cout<<")\n";
			}

		  // Define transitions, referring back to earlier specified values
		  std::cout<<"\n";
		  for (state1 = branch.states.begin(); state1 != branch.states.end(); state1++)
			{
			  if (branch.get_state_type(*state1) == "E") continue; 
			  outgoing = branch.get_outgoing(*state1); 
			  for (state2 = outgoing.begin(); state2 != outgoing.end(); state2++)
				{
				  std::cout<<"\t(transition (from "<< branch.get_state_name(*state1) << ") ";
				  std::cout<<"(to "<< branch.get_state_name(*state2) << ") ";
				  std::cout<< "(label " << child << "_"<<branch.get_state_name(*state1)<<"_"<<branch.get_state_name(*state2)<<" ";
				  std::cout<<"))\n";
				}
			} 
		  std::cout<<");; end transducer " << child << "\n\n";
		  
		}
	}
  vector<node> rootsKids; 
  for_rooted_children(tree, tree.root , child)
	rootsKids.push_back(*child);

  std::cout<<"(branch (name ROOT)\n\t(from SUBROOT) (to root)\n\t(transducer ROOT) \n\t";

  std::cout<< show_branch(rootsKids[0]) ;
  std::cout<< show_branch(rootsKids[1]);
  std::cout<< ");; end phylogenetic tree"; 
}

string Reconstruction::show_branch(node startNode)
{
  string out; 
  vector<node> kids; 
  out += "(branch (name " + string(tree.node_name[startNode].c_str()) + ")\n";

  out += "\t(from " + string(tree.node_name[tree.parent[startNode]].c_str()) + ") ";
  out += "(to " + string(tree.node_name[startNode].c_str()) + ") \n";  

  out += "\t(transducer " + string(tree.node_name[startNode].c_str()) + ")\n";

  if (tree.is_internal(startNode))
	{
	    for_rooted_children(tree, startNode , child)
		  kids.push_back(*child);
		out += show_branch(kids[0]);
		out += show_branch(kids[1]); 
	}		
  out += "\t);; end branch " + string(tree.node_name[startNode].c_str()) +  " \n\n";
  return out; 
}

void Reconstruction::simulate_alignment(Alphabet alphabet, Irrev_EM_matrix rate_matrix)
{
  map<node, Digitized_biosequence> sequences; 
  Digitized_biosequence parentSeq; 
  string childName;
  Decomposition decomp; 
  SingletTrans R(alphabet, rate_matrix);
  //write the tree in stockolm style
  std::cout<<"#=GF NH\t";
  tree.write(std::cout, 0); 
  
  // First, simulate the sequences via a set of pairwise alignments on the tree.  decomp is filled in the calls to sample_pairwise
  for_nodes_pre (tree, tree.root, -1, bi)
    {
      const Phylogeny::Branch& b = *bi;
      node treeNode = b.second;
      if (loggingLevel >= 2)
	std::cout<<"visiting node:" << treeNode<<endl; 
      if (treeNode == tree.root)
	sequences[treeNode] = sample_root(R); 
      else
	{
	  // construct a branch transducer using the appropriate branch length.
	  BranchTrans branch(tree.branch_length(b.first, b.second), alphabet, rate_matrix, ins_rate, del_rate, gap_extend, sub_rate); 
	  parentSeq = sequences[b.first]; 
	  childName = string( tree.node_name[b.second].c_str() ); 	  
	  sequences[treeNode] = sample_pairwise(parentSeq, branch, b.first, b.second, decomp);
	}
    }
  
  // Stitch together the pairwise alignments via the alignment class.  This is fairly trivial since we've added pairwise paths to 
  // decomp as we made them
  Alignment_path multiple; 
  multiple.compose(decomp, false); 

  Alignment align(multiple); 
  align.row_name = tree.node_name; 
  
  Score_profile dummySP; 
  vector<Score_profile> seqs(tree.nodes()); 
  // Sequence data is stored in Alignment.prof, a vector of pointer to Score_profile
  Biosequence s; 
  for (Node node  = 0; node < tree.nodes(); node++)
    {
      dummySP = alphabet.dsq2score(sequences[node], dummySP);
      seqs[node] = dummySP;
      align.prof[node] = &(seqs[node]);
    }
  // Display the resulting alignment
  align.write_MUL(std::cout, alphabet);  
}

Digitized_biosequence Reconstruction::sample_root(SingletTrans R)
{
  unsigned int s = 0;  // 0 is the start state
  string sType;
  vector<state>::iterator sIter; 
  Digitized_biosequence childSeq;  // it's called childseq even though it's the root sequence which has no parents.  
  vector<state> outgoing; 
  vector<double> outgoingWeights; 
  int sampled; 

  // The root sequence's length is specified on the cmd line - more control in simulation
  // NB this assumes there is only one Insert state in the root transducer...not always the case.  
  // In the future we could instead alter the while loop below, to increment a counter each time an insert state
  // is reached, and terminate whenever the specified root length is reached.  
  if (rootLength != -1) 
	{
	  for (s=0; s<R.states.size(); s++)
		if (R.get_state_type(s) == "I")
		  outgoingWeights = R.get_emission_distribution(s);
	  for (int sampled=0; sampled< rootLength; sampled++)
	    childSeq.push_back(sample(outgoingWeights));
	  return childSeq; 
	}
		  

  // Otherwise, sample directly from the root transducer.  
  while (s != R.states.size()-1) // not the end state
    {
      outgoing = R.get_outgoing(s); 
      outgoingWeights.clear();
      for (sIter = outgoing.begin(); sIter != outgoing.end(); sIter++)
	outgoingWeights.push_back( R.get_transition_weight(s, *sIter) );
      //The new state is sampled
      sampled = sample(outgoingWeights)	  ;
      s = outgoing[sampled]; 
      sType = R.get_state_type(s); 
      
      // If we're at an insert state, sample another state
      if  (sType == "I")
	{
	  outgoingWeights = R.get_emission_distribution(s);
	  childSeq.push_back(sample(outgoingWeights));
	}
    }
  return childSeq;
}
  
Digitized_biosequence Reconstruction::sample_pairwise(Digitized_biosequence parentSeq, BranchTrans branch, Node parent, Node child, Decomposition& decomp )
{
  state s = 0, sNew, endState = branch.states.size()-1;  // 0 is the start state
  string sType;
  vector<state>::iterator sIter; 
  Digitized_biosequence childSeq; 
  vector<state> outgoing, possible; 
  vector<double> outgoingWeights; 
  unsigned int inIdx = 0, incomingCharacter; 

  Row_pair row_pair; 
  row_pair.first = parent; row_pair.second = child; 
  Row row0, row1;
  
  while (s != endState) // not the end state
    {
      outgoing.clear();
      possible = branch.get_outgoing(s); 
      for (sIter=possible.begin(); sIter!=possible.end(); sIter++)
	{
	  if (inIdx != parentSeq.size())
	    {
	      //we can only transition to end state if we've used up all the input characters
	      if (branch.get_state_type(*sIter) != "E") 
		outgoing.push_back(*sIter);
	    }
	  else
	    //Similarly, after the input characters have been exhausted, we can't go to match or delete
	    if (branch.get_state_type(*sIter) != "M" && (branch.get_state_type(*sIter) != "D") ) 
	      outgoing.push_back(*sIter);
	}
      
      outgoingWeights.clear(); 
      for (sIter = outgoing.begin(); sIter != outgoing.end(); sIter++)
	outgoingWeights.push_back( branch.get_transition_weight(s, *sIter) );
      
      
      // This added bit is due to a slight quirk in the transducer structure that's used.  
      // For soomewhat tricky reasons, there can only be one wait state with a  wait-end transitions
      // Thus the other wait states can never reach the end state/terminate so we must manually add such a 
      // transition here.  This comes up with the affine gap transducer (which is default), since it has two
      // wait states, but only one has a wait -> end transition. 
      
      if (inIdx == parentSeq.size() && branch.get_state_type(s) == "W")
	if ( !in( endState, outgoing) )
	  {
	    outgoing.push_back(endState); 
	    outgoingWeights.push_back(1.0); 
	  }
      
      
      //The new state is sampled
      sNew = outgoing[sample(outgoingWeights)]; 
      if (branch.get_state_type(s) == "W" && branch.get_state_type(sNew) == "W") // this is no good...there are no real W->W transitions!
	{
	  std::cerr<<"Wait-wait transition, problem!\n Outgoing size, states, possible: "<<outgoing.size() << " , ";
	  std::cerr<<"outgoing states"; displayVector(outgoing);
	  std::cerr<<"\n";
	  std::cerr<<"outgoingWeights: "; displayVector(outgoingWeights);
	  std::cerr<<"\n";
	  std::cerr<<"possible states"; displayVector(possible);
	  exit(1); 
	}
      
      s = sNew; 
      sType = branch.get_state_type(s); 
      if (loggingLevel >=2)
	std::cerr<<"Pairwise: new state: "<<s<<"  type: "<<branch.get_state_type(s)<<endl; 
      
      if  (sType == "I") // insert state: child gets a character with a gap at the parent.  
	{
	  outgoingWeights = branch.get_emission_distribution(s);
	  childSeq.push_back(sample(outgoingWeights)); 
	  row0.push_back(false); 
	  row1.push_back(true); 
	}
      
      else if (sType == "M")  // match state: child gets a character sampled conditional on parent's character
	{
	  incomingCharacter = parentSeq[inIdx];
	  outgoingWeights.clear(); 
	  for (int charIdx=0; charIdx < branch.alphabet_size; charIdx++)
	    outgoingWeights.push_back(branch.get_match_weight(s, incomingCharacter, charIdx) );
	  childSeq.push_back(sample(outgoingWeights));
	  inIdx++; 
	  row0.push_back(true); 
	  row1.push_back(true); 
	}
      else if ( sType == "D") // delete state: child gets a gap, parent has a character
	{
	  row0.push_back(true);  
	  row1.push_back(false); 
	  inIdx++;
	}
            
    }

  // Make a DART-style alignment, and insert it into the big decomposition that will eventually become a multiple alignment
  Alignment_path pairAlign(0,0);
  pairAlign.insert_rows(0,row0);
  pairAlign.insert_rows(1,row1); 
  decomp[row_pair] = pairAlign; 
  return childSeq;
}

double Reconstruction::get_root_ins_estimate(void)
{
  if (!sequences.size())
    {
      std::cerr<<"Error: no sequences, can't estimate root length distribution!\n";
      return root_insert_prob; 
    }
  else
    {
      int totalLength = 0;
      for (map<string,string>::iterator seqIter= sequences.begin(); seqIter!=sequences.end(); seqIter++)
	totalLength += (seqIter->second).size();
      double avg = double(totalLength)/double(sequences.size());
      return 1.0-(1.0 / avg);
    }
}
	
// void Reconstruction::show_indels(Stockholm stock)
// {
//   ofstream myfile;
//   myfile.open (indel_filename.c_str());
//   myfile << "Logging indel information is not yet implemented.\n";
//   myfile.close();
// //   node parent, child; 
  
// //   vector<Row_pair> row_pairs; 
  
// //   for_nodes_post (tree, tree.root, -1, bi)
// //     {
// //       const Phylogeny::Branch& b = *bi;
// //       row_pairs.push_back(b); 
// //       if (b.second == tree.root) continue;
// //       std::cout<<"Adding to pairs: " << tree.node_name[b.first] << " and " <<tree.node_name[b.second]<<endl; 
// //     }
// //   Decomposition decomp = stock.path.decompose(row_pairs); 
// }
