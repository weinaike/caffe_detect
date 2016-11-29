#include "ctrain.h"

CTrain::CTrain(string solverfile,string weightfile, string snapshotfile)
{
    m_solverfile=solverfile;
    m_weightfile=weightfile;
    m_snapshotfile=snapshotfile;
    CHECK_GT(m_solverfile.size(), 0) << "Need a solver definition to train.";
    CHECK(!m_snapshotfile.size() || !m_weightfile.size())
        << "Give a snapshot to resume training or weights to finetune but not both.";
}

CTrain::CTrain(string solverfile)
{
    m_solverfile=solverfile;
    CHECK_GT(m_solverfile.size(), 0) << "Need a solver definition to train.";
}


int CTrain::train() {

  caffe::SolverParameter solver_param;
  caffe::ReadSolverParamsFromTextFileOrDie(m_solverfile, &solver_param);
  Caffe::set_mode(Caffe::CPU);
  caffe::shared_ptr<caffe::Solver<float> >
      solver(caffe::SolverRegistry<float>::CreateSolver(solver_param));

  if (m_snapshotfile.size()) {
    LOG(INFO) << "Resuming from " << m_snapshotfile;
    solver->Restore(m_snapshotfile.c_str());
  } else if (m_weightfile.size()) {
    CopyLayers(solver.get(), m_weightfile);
  }

  LOG(INFO) << "Starting Optimization";
  solver->Solve();
  LOG(INFO) << "Optimization Done.";
  return 0;
}


void CTrain::CopyLayers(caffe::Solver<float>* solver, const std::string& model_list) {
  std::vector<std::string> model_names;
  boost::split(model_names, model_list, boost::is_any_of(",") );
  for (unsigned i = 0; i < model_names.size(); ++i) {
    LOG(INFO) << "Finetuning from " << model_names[i];
    solver->net()->CopyTrainedLayersFrom(model_names[i]);
    for (unsigned j = 0; j < solver->test_nets().size(); ++j) {
      solver->test_nets()[j]->CopyTrainedLayersFrom(model_names[i]);
    }
  }
}
