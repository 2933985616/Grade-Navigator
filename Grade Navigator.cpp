#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <cstdlib> // for atof

using namespace std;

const float SCORE_THRESHOLD = 60.0; // �쳣�ɼ���ֵ

// �γ���
class Course {
public:
    string courseName;
    float score;
    float credit;
    
    Course() {}
    Course(const string &name, float s, float c) : courseName(name), score(s), credit(c) {}
};

// ѧ����
class Student {
public:
    string id;
    string name;
    vector<Course> courses;
    
    Student() {}
    Student(const string &id_, const string &name_) : id(id_), name(name_) {}
    
    // ��ӿγ�
    void addCourse(const Course &course) {
        courses.push_back(course);
    }
    
    // GPA��̬���㣺����Ȩ��ƽ���㷨
    float calculateGPA() const {
        float totalWeightedGrade = 0;
        float totalCredits = 0;
        for (vector<Course>::const_iterator it = courses.begin(); it != courses.end(); ++it) {
            float gradePoint = convertScoreToGradePoint(it->score);
            totalWeightedGrade += gradePoint * it->credit;
            totalCredits += it->credit;
        }
        return (totalCredits == 0) ? 0 : totalWeightedGrade / totalCredits;
    }
    
    // ������ת��Ϊ���㣨�򵥷ֶ�ת����
    float convertScoreToGradePoint(float score) const {
        if (score >= 90) return 4.0;
        else if (score >= 80) return 3.0;
        else if (score >= 70) return 2.0;
        else if (score >= 60) return 1.0;
        else return 0.0;
    }
    
    // ����Ƿ�����쳣�ɼ���������ֵ��
    bool hasAbnormalScore() const {
        for (vector<Course>::const_iterator it = courses.begin(); it != courses.end(); ++it) {
            if (it->score < SCORE_THRESHOLD)
                return true;
        }
        return false;
    }
};

// ģ�ͣ�ѧ���ֿ⣬�������ݴ洢�͹���
class StudentRepository {
private:
    map<string, Student> students;
public:
    // ��ӻ򸲸�ѧ����Ϣ
    void addStudent(const Student &student) {
        students[student.id] = student;
    }
    
    // ɾ��ѧ��
    bool deleteStudent(const string &id) {
        return students.erase(id) > 0;
    }
    
    // ����ѧ����Ϣ
    bool updateStudent(const Student &student) {
        if (students.find(student.id) != students.end()){
            students[student.id] = student;
            return true;
        }
        return false;
    }
    
    // ����ID��ѯѧ��
    Student* getStudentById(const string &id) {
        map<string, Student>::iterator it = students.find(id);
        return (it != students.end()) ? &(it->second) : NULL;
    }
    
    // ��ȡ����ѧ������
    const map<string, Student>& getAllStudents() const {
        return students;
    }
    
    // ��txt�ļ���������
    // ��ʽʾ����id,name;courseName,score,credit|courseName,score,credit|...
    void loadFromFile(const string &filename) {
        ifstream infile(filename.c_str());
        if (!infile) {
            cout << "�ļ� " << filename << " ��ʧ�ܣ�" << endl;
            return;
        }
        students.clear();
        string line;
        while(getline(infile, line)){
            if(line.empty()) continue;
            stringstream ss(line);
            string header;
            getline(ss, header, ';');
            stringstream hs(header);
            string id, name;
            getline(hs, id, ',');
            getline(hs, name, ',');
            Student student(id, name);
            string coursesStr;
            if(getline(ss, coursesStr)){
                stringstream cs(coursesStr);
                string courseEntry;
                while(getline(cs, courseEntry, '|')){
                    if(courseEntry.empty()) continue;
                    stringstream courseStream(courseEntry);
                    string courseName, scoreStr, creditStr;
                    getline(courseStream, courseName, ',');
                    getline(courseStream, scoreStr, ',');
                    getline(courseStream, creditStr, ',');
                    float score = atof(scoreStr.c_str());
                    float credit = atof(creditStr.c_str());
                    student.addCourse(Course(courseName, score, credit));
                }
            }
            students[student.id] = student;
        }
        infile.close();
        cout << "���ݼ��سɹ���" << endl;
    }
    
    // �������ݵ�txt�ļ�
    void saveToFile(const string &filename) {
        ofstream outfile(filename.c_str());
        if (!outfile) {
            cout << "�ļ� " << filename << " ��ʧ�ܣ�" << endl;
            return;
        }
        for (map<string, Student>::const_iterator it = students.begin(); it != students.end(); ++it) {
            const Student &student = it->second;
            outfile << student.id << "," << student.name << ";";
            for (size_t i = 0; i < student.courses.size(); i++){
                const Course &course = student.courses[i];
                outfile << course.courseName << "," << course.score << "," << course.credit;
                if(i != student.courses.size()-1)
                    outfile << "|";
            }
            outfile << "\n";
        }
        outfile.close();
        cout << "���ݱ���ɹ���" << endl;
    }
};

// ������������ҵ���߼�
class StudentController {
private:
    StudentRepository repository;
public:
    // ���ѧ��
    void addStudent() {
        string id, name;
        cout << "������ѧ��ID: ";
        cin >> id;
        cout << "������ѧ������: ";
        cin >> name;
        Student student(id, name);
        int courseCount;
        cout << "������γ�����: ";
        cin >> courseCount;
        for (int i = 0; i < courseCount; i++){
            string courseName;
            float score, credit;
            cout << "������� " << i + 1 << " �ſγ�����: ";
            cin >> courseName;
            cout << "������ÿγ̳ɼ�: ";
            cin >> score;
            cout << "������ÿγ�ѧ��: ";
            cin >> credit;
            student.addCourse(Course(courseName, score, credit));
        }
        repository.addStudent(student);
        cout << "ѧ����ӳɹ���" << endl;
        // Ԥ���������쳣�ɼ�
        if (student.hasAbnormalScore()){
            cout << "ע�⣺��ѧ�������쳣�ɼ�������" << SCORE_THRESHOLD << "�֣���" << endl;
        }
    }
    
    // ɾ��ѧ��
    void deleteStudent() {
        string id;
        cout << "������Ҫɾ����ѧ��ID: ";
        cin >> id;
        if (repository.deleteStudent(id))
            cout << "ѧ��ɾ���ɹ���" << endl;
        else
            cout << "δ�ҵ���ѧ����" << endl;
    }
    
    // �޸�ѧ����Ϣ
    void updateStudent() {
        string id;
        cout << "������Ҫ�޸ĵ�ѧ��ID: ";
        cin >> id;
        Student* student = repository.getStudentById(id);
        if (student == NULL){
            cout << "δ�ҵ���ѧ����" << endl;
            return;
        }
        cout << "��ǰ������" << student->name << "����������������";
        cin >> student->name;
        int choice;
        cout << "�Ƿ��޸Ŀγ���Ϣ����1-�ǣ�0-�񣩣�";
        cin >> choice;
        if (choice == 1){
            student->courses.clear();
            int courseCount;
            cout << "������γ�����: ";
            cin >> courseCount;
            for (int i = 0; i < courseCount; i++){
                string courseName;
                float score, credit;
                cout << "������� " << i + 1 << " �ſγ�����: ";
                cin >> courseName;
                cout << "������ÿγ̳ɼ�: ";
                cin >> score;
                cout << "������ÿγ�ѧ��: ";
                cin >> credit;
                student->addCourse(Course(courseName, score, credit));
            }
        }
        repository.updateStudent(*student);
        cout << "ѧ����Ϣ���³ɹ���" << endl;
        if (student->hasAbnormalScore()){
            cout << "ע�⣺��ѧ�������쳣�ɼ�������" << SCORE_THRESHOLD << "�֣���" << endl;
        }
    }
    
    // ��ѯѧ����Ϣ
    void queryStudent() {
        string id;
        cout << "������Ҫ��ѯ��ѧ��ID: ";
        cin >> id;
        Student* student = repository.getStudentById(id);
        if (student == NULL){
            cout << "δ�ҵ���ѧ����" << endl;
            return;
        }
        cout << "ѧ��ID: " << student->id << "\n����: " << student->name << endl;
        cout << "�γ���Ϣ��" << endl;
        for (vector<Course>::const_iterator it = student->courses.begin(); it != student->courses.end(); ++it) {
            cout << "�γ�����: " << it->courseName 
                 << ", �ɼ�: " << it->score 
                 << ", ѧ��: " << it->credit;
            if (it->score < SCORE_THRESHOLD)
                cout << " [�쳣�ɼ�]";
            cout << endl;
        }
        cout << fixed << setprecision(2);
        cout << "����õ���GPA: " << student->calculateGPA() << endl;
    }
    
    // �г�����ѧ����Ϣ
    void listAllStudents() {
        const map<string, Student>& students = repository.getAllStudents();
        if (students.empty()){
            cout << "����ѧ�����ݣ�" << endl;
            return;
        }
        for (map<string, Student>::const_iterator it = students.begin(); it != students.end(); ++it) {
            const Student &student = it->second;
            cout << "---------------------------" << endl;
            cout << "ѧ��ID: " << student.id << "\n����: " << student.name << endl;
            cout << "�γ���Ϣ��" << endl;
            for (vector<Course>::const_iterator jt = student.courses.begin(); jt != student.courses.end(); ++jt) {
                cout << "�γ�����: " << jt->courseName 
                     << ", �ɼ�: " << jt->score 
                     << ", ѧ��: " << jt->credit;
                if (jt->score < SCORE_THRESHOLD)
                    cout << " [�쳣�ɼ�]";
                cout << endl;
            }
            cout << "GPA: " << fixed << setprecision(2) << student.calculateGPA() << endl;
        }
        cout << "---------------------------" << endl;
    }
    
    // �������ݵ��ļ�
    void saveData() {
        string filename;
        cout << "�����뱣���ļ��������� students.txt��: ";
        cin >> filename;
        repository.saveToFile(filename);
    }
    
    // ���ļ���������
    void loadData() {
        string filename;
        cout << "����������ļ��������� students.txt��: ";
        cin >> filename;
        repository.loadFromFile(filename);
    }
}; // ���� StudentController ��

// ��ͼ����ʾ�˵�
void showMenu() {
    cout << "\n========== ѧ���ɼ������ն� ==========" << endl;
    cout << "1. ���ѧ��" << endl;
    cout << "2. ɾ��ѧ��" << endl;
    cout << "3. �޸�ѧ����Ϣ" << endl;
    cout << "4. ��ѯѧ����Ϣ" << endl;
    cout << "5. �г�����ѧ��" << endl;
    cout << "6. ��������" << endl;
    cout << "7. ��������" << endl;
    cout << "0. �˳�" << endl;
    cout << "��ѡ�����: ";
}

int main() {
    StudentController controller;
    int choice;
    while (true) {
        showMenu();
        cin >> choice;
        switch (choice) {
            case 1:
                controller.addStudent();
                break;
            case 2:
                controller.deleteStudent();
                break;
            case 3:
                controller.updateStudent();
                break;
            case 4:
                controller.queryStudent();
                break;
            case 5:
                controller.listAllStudents();
                break;
            case 6:
                controller.saveData();
                break;
            case 7:
                controller.loadData();
                break;
            case 0:
                cout << "�˳�ϵͳ���ټ���" << endl;
                return 0;
            default:
                cout << "��Чѡ�������ѡ��" << endl;
                break;
        }
    }
    return 0;
}

