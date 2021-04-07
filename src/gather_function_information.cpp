#include <gather_function_information.hpp>
int main(int argc, char** argv){
    GatherFunctionInformationPolicy* functionInformationGatherer = new GatherFunctionInformationPolicy();
    srcSAXController control(argv[1]);
    srcSAXEventDispatch::srcSAXEventDispatcher<> handler({functionInformationGatherer}, false);
    try{
      control.parse(&handler); //Start parsing
    }catch(SAXError e){
      std::cerr<<"ERROR: "<<e.message;
    }
	return 0;
}