pipeline {
    agent any

    stages {
        stage('Checkout') {
            steps {
                git 'https://github.com/seabears/Jenkins-Test.git'
            }
        }
        stage('Build') {
            steps {
                sh 'gcc -o test_program main.c'
            }
        }
        stage('Test') {
            steps {
                sh './test_program'
            }
        }
    }
}
