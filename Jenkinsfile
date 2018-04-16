pipeline {

    options {
        timestamps()
    }
    
    agent {
        node {
            label 'ubuntu-build'
            }   // end of node
    }    // end of agent  
    
    stages {

            stage ('Checkout') {
                steps {
                    checkout scm
                    //git 'https://github.com/yackey/AEF-CI.git'
                    stash name: 'everything', includes: '**'
                }   // end of steps
            }   // end of checkout stage  
            
            stage ('Parallel Build') {
                failFast true
                parallel {

                    stage ('Release ARM Build') {

                        agent {
                            node {
                                label 'ubuntu-build'
                                }   // end of node
                        }    // end of agent

                        steps {
                            deleteDir()
                            unstash 'everything'
                            dir ("build") {
                                Build_ARM_Release()
                            }   // end of dir
                            deleteDir()
                        }   // end of steps
                    }   // end of Release ARM Build stage

                    stage ('Debug ARM Build') {

                        agent {
                            node {
                                label 'ubuntu-build'
                                }   // end of node
                        }    // end of agent

                        steps {
                            deleteDir()
                            unstash 'everything'
                            dir ("build") {
                                Build_ARM_Debug()
                            }   // end of dir
                            deleteDir()
                        }   // end of steps
                    }   // end of Debug ARM Build stage

                    stage ('Release Simulator Build') {

                        agent {
                            node {
                                label 'ubuntu-build'
                                }   // end of node
                        }    // end of agent

                        steps {
                            deleteDir()
                            unstash 'everything'
                            dir ("build") {
                                Build_x86_64_Release()
                            }   // end of dir
                            deleteDir()
                        }   // end of steps
                    }   // end of Release Simulator Build stage
                    
                    stage ('Debug Simulator Build') {

                        agent {
                            node {
                                label 'ubuntu-build'
                                }   // end of node
                        }    // end of agent

                        steps {
                            deleteDir()
                            unstash 'everything'
                            dir ("build") {                           
                                Build_x86_64_Debug()
                            }   // end of dir
                            stash name: 'debug_simulator_build_stash', includes: '**'
                        }   // end of steps
                    }   // end of Debug Simulator Build stage                    
                    
                }   //end of parallel
                
            }   // end of parallel build stage

            stage("Generate Test results") {
                agent {
                    node {
                        label 'ubuntu-build'
                    }   // end of node
                }    // end of agent
                
                steps {
                    deleteDir()
                    unstash 'debug_simulator_build_stash'
                    dir ("build") {                      
                        GenerateTestResuts_x86_64_Debug()
                     }   // end of dir
                     stash name: 'debug_simulator_test_stash', includes: '**'
                }   // end of steps
            }   // end of Generate Test results            
            
            stage("Publish Test results") {
                agent {
                    node {
                        label 'ubuntu-build'
                    }   // end of node
                }    // end of agent
                
                steps {
                    deleteDir()
                    unstash 'debug_simulator_test_stash'
                    //dir ("build") {
                        PublishTestResuts_x86_64_Debug()
                     //}   // end of dir
                }   // end of steps
            }   // end of Publish Test results       
            
            stage("Upload Packages") {
                agent {
                    node {
                        label 'ubuntu-build'
                    }   // end of node
                }    // end of agent
                
                steps {
                    dir ("build") {
                        UploadPackages()
                     }   // end of dir
                }   // end of steps
                
            }   // end of Upload Packages
            
    }   // end of stages        
    
    post {
        always {
            RemovePackages()
        }
    }    
}   // end of pipeline


def UploadPackages() {
        sh 'conan user -p Dapassword123 -r YackNet yackey'
        sh 'conan upload AEFLib/1.0@yackey/stable --all -r YackNet --confirm'
}

def RemovePackages() {
        sh 'conan remove AEFLib/1.0@yackey/stable -f'
}

def Build_x86_64_Release() {
    sh 'conan install ../test -s os=Linux -s arch=x86_64 -s compiler=gcc -s compiler.version=6.4 -s build_type=Release'
    sh 'cmake -G "Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_TOOLCHAIN_FILE=../Toolchain-GCC-x86_64.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TARGET_PROCESSOR=x86_64 ..'
    sh 'cmake --build .'
    //
    sh 'rm ./conan*.*'
    sh 'conan export-pkg .. AEFLib/1.0@yackey/stable -s os=Linux -s arch=x86_64 -s compiler=gcc -s compiler.version=6.4 -s build_type=Release -f'
}

def Build_x86_64_Debug() {
    sh 'conan install ../test -s os=Linux -s arch=x86_64 -s compiler=gcc -s compiler.version=6.4 -s build_type=Debug'
    sh 'cmake -G "Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_TOOLCHAIN_FILE=../Toolchain-GCC-x86_64.cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TARGET_PROCESSOR=x86_64 ..'
    sh 'cmake --build .'
    //
    sh 'rm ./conan*.*'
    sh 'conan export-pkg .. AEFLib/1.0@yackey/stable -s os=Linux -s arch=x86_64 -s compiler=gcc -s compiler.version=6.4 -s build_type=Debug -f'
}

def Build_ARM_Release() {
    sh 'conan install ../test -s os=Linux -s arch=armv7 -s compiler=gcc -s compiler.version=6.4 -s build_type=Release'
    sh 'cmake -G "Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_TOOLCHAIN_FILE=../Toolchain-GCC-ARM.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TARGET_PROCESSOR=ARM ..'
    sh 'cmake --build .'
    //
    sh 'rm ./conan*.*'
    sh 'conan export-pkg .. AEFLib/1.0@yackey/stable -s os=Linux -s arch=armv7 -s compiler=gcc -s compiler.version=6.4 -s build_type=Release -f'
}

def Build_ARM_Debug() {
    sh 'conan install ../test -s os=Linux -s arch=armv7 -s compiler=gcc -s compiler.version=6.4 -s build_type=Debug'
    sh 'cmake -G "Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_TOOLCHAIN_FILE=../Toolchain-GCC-ARM.cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TARGET_PROCESSOR=ARM ..'
    sh 'cmake --build .'
    //
    sh 'rm ./conan*.*'
    sh 'conan export-pkg .. AEFLib/1.0@yackey/stable -s os=Linux -s arch=armv7 -s compiler=gcc -s compiler.version=6.4 -s build_type=Debug -f'
}

def GenerateTestResuts_x86_64_Debug() {
    sh './x86_64/bin/Debug/AefTest --gtest_output=xml:AefTestResultsDebug.xml'                        
}

def PublishTestResuts_x86_64_Debug() {
    sh 'pwd'
    step([$class: 'JUnitResultArchiver', allowEmptyResults: true, testResults: 'AefTestResultsDebug.xml'])
    //  
    sh '/usr/bin/lcov --directory . --capture --output-file ./code_coverage.info -rc lcov_branch_coverage=1' 
    sh '/usr/bin/genhtml code_coverage.info --branch-coverage --output-directory ./code_coverage_report_Debug'                        
    //
    publishHTML([allowMissing: false, 
        alwaysLinkToLastBuild: false, 
        keepAll: true, 
        reportDir: 'code_coverage_report_Debug', 
        reportFiles: 'index.html', 
        reportName: 'Code Coverage Debug', 
        reportTitles: ''])                         
}
