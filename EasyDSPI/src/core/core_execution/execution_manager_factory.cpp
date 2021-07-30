﻿/*
Copyright 2021 University of Manchester

Licensed under the Apache License, Version 2.0(the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http:  // www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "execution_manager_factory.hpp"

#include "execution_manager.hpp"
#include "init_state.hpp"

using easydspi::core::core_execution::ExecutionManager;
using easydspi::core::core_execution::ExecutionManagerFactory;
using easydspi::core_interfaces::ExecutionManagerInterface;
using easydspi::dbmstodspi::InitState;

std::unique_ptr<ExecutionManagerInterface>
ExecutionManagerFactory::getManager() {
  return std::make_unique<ExecutionManager>(std::make_unique<InitState>());
}