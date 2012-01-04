SET (REL_PATH ${ASE_SOURCE_DIR}/src)
SET (CONTROL_DIR ${REL_PATH}/control)
SET (TOOLS_DIR ${REL_PATH}/tools)
SET (UTILS_DIR ${REL_PATH}/utils)


SET (FULL_SRCS 
        ${REL_PATH}/tools/Timer/TimerManager.c
        ${REL_PATH}/tools/IDContainer/IDContainer.c
        ${REL_PATH}/tools/Runge-Kutta/RK4.c
        ${REL_PATH}/tools/RewardCollector/RewardCollector.c
        ${REL_PATH}/tools/LinkedList/LinkedList.c
        ${REL_PATH}/control/strategies/Cpg/CpgSuper.c
        ${REL_PATH}/control/strategies/Cpg/CpgSuperManager.c
        ${REL_PATH}/control/strategies/Cpg/CnoCpg/CnoCpg.c
        ${REL_PATH}/control/strategies/Cpg/CnoCpg/CnoCpgManager.c
        ${REL_PATH}/control/strategies/Cpg/HopfCartesianCpg/HopfCartesianCpg.c
        ${REL_PATH}/control/strategies/Cpg/HopfCartesianCpg/HopfCartesianCpgManager.c
        ${REL_PATH}/control/strategies/Learning/Learning.c
        ${REL_PATH}/control/strategies/Learning/LearningManager.c
        ${REL_PATH}/control/strategies/SPSALearning/SPSALearning.c
        ${REL_PATH}/control/strategies/RandomSearch/RandomSearch.c
        ${REL_PATH}/control/strategies/kNN/kNN.c
        ${REL_PATH}/control/strategies/ANN/ann.c  
        ${REL_PATH}/control/strategies/ffANN/ffANN.c  
        ${REL_PATH}/control/strategies/GA/ga.c  
        ${REL_PATH}/control/strategies/PSO/pso.c  
        ${REL_PATH}/control/strategies/Playback/Playback.c
        ${REL_PATH}/control/strategies/Consensus/ConsensusAverage.c
        ${REL_PATH}/control/strategies/DistributedStateMachine/DistributedStateManager.c
        ${REL_PATH}/control/strategies/DistributedStateMachine/DistributedStateMerger.c
        ${REL_PATH}/control/arbitration/Subsumption/Subsumption.c
        ${REL_PATH}/communication/Broadcast/Broadcaster.c
        ${REL_PATH}/communication/RemoteControl/ModularCommander/MCManager.c
        ${REL_PATH}/communication/Gossip/GossipManager.c
        ${REL_PATH}/communication/StateSharing/StateSharing.c
        ${REL_PATH}/communication/SharedTimer/SharedTimer.c
        ${REL_PATH}/control/behaviors/generic/IDManager_MC/IDManager_MC.c
        ${REL_PATH}/control/behaviors/generic/CpgGait/CpgGait.c
        ${REL_PATH}/control/behaviors/generic/LearningCpgGait/LearningCpgGait.c
        ${REL_PATH}/control/behaviors/generic/LearningCpgGait/SPSACpgGait/SPSACpgGait.c
        ${REL_PATH}/control/behaviors/generic/LearningCpgGait/RandomSearchCpgGait/RandomSearchCpgGait.c
        ${REL_PATH}/control/behaviors/generic/LegoUserInterface/LegoUserInterface.c
        ${REL_PATH}/control/behaviors/generic/LegoUserInterface/LuiBoard.c
        ${REL_PATH}/control/behaviors/generic/LegoUserInterface/LuiEventManager.c
        ${REL_PATH}/control/behaviors/generic/LegoUserInterface/LuiBehaviorManager.c
        ${REL_PATH}/control/behaviors/generic/LegoUserInterface/LuiManager.c
        ${REL_PATH}/control/behaviors/generic/LegoUserInterface/LuiTraining.c
        ${REL_PATH}/control/strategies/Gradient/Gradient.c
        ${REL_PATH}/control/strategies/Gradient/GradientManager.c
        ${REL_PATH}/infrastructure/EventManager/EventManager.c
        ${REL_PATH}/infrastructure/Scheduler/Scheduler.c
        ${REL_PATH}/infrastructure/Entity/Entity.c 
        ${REL_PATH}/infrastructure/Entity/Manager/Manager.c 
        ${REL_PATH}/infrastructure/Entity/Manager/PubSubManager/PubSubManager.c 
        ${REL_PATH}/infrastructure/Entity/Strategy/Strategy.c 
        ${REL_PATH}/infrastructure/Entity/Strategy/PeriodicStrategy/PeriodicStrategy.c 
        ${REL_PATH}/infrastructure/MemManager/MemManager.c 
        ${REL_PATH}/infrastructure/MsgManager/MsgManager.c
)

