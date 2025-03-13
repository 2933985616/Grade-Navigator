#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <cstdlib> // for atof

using namespace std;

const float SCORE_THRESHOLD = 60.0; // 异常成绩阈值

// 课程类
class Course {
public:
    string courseName;
    float score;
    float credit;
    
    Course() {}
    Course(const string &name, float s, float c) : courseName(name), score(s), credit(c) {}
};

// 学生类
class Student {
public:
    string id;
    string name;
    vector<Course> courses;
    
    Student() {}
    Student(const string &id_, const string &name_) : id(id_), name(name_) {}
    
    // 添加课程
    void addCourse(const Course &course) {
        courses.push_back(course);
    }
    
    // GPA动态计算：采用权重平均算法
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
    
    // 将分数转换为绩点（简单分段转换）
    float convertScoreToGradePoint(float score) const {
        if (score >= 90) return 4.0;
        else if (score >= 80) return 3.0;
        else if (score >= 70) return 2.0;
        else if (score >= 60) return 1.0;
        else return 0.0;
    }
    
    // 检查是否存在异常成绩（低于阈值）
    bool hasAbnormalScore() const {
        for (vector<Course>::const_iterator it = courses.begin(); it != courses.end(); ++it) {
            if (it->score < SCORE_THRESHOLD)
                return true;
        }
        return false;
    }
};

// 模型：学生仓库，负责数据存储和管理
class StudentRepository {
private:
    map<string, Student> students;
public:
    // 添加或覆盖学生信息
    void addStudent(const Student &student) {
        students[student.id] = student;
    }
    
    // 删除学生
    bool deleteStudent(const string &id) {
        return students.erase(id) > 0;
    }
    
    // 更新学生信息
    bool updateStudent(const Student &student) {
        if (students.find(student.id) != students.end()){
            students[student.id] = student;
            return true;
        }
        return false;
    }
    
    // 根据ID查询学生
    Student* getStudentById(const string &id) {
        map<string, Student>::iterator it = students.find(id);
        return (it != students.end()) ? &(it->second) : NULL;
    }
    
    // 获取所有学生数据
    const map<string, Student>& getAllStudents() const {
        return students;
    }
    
    // 从txt文件加载数据
    // 格式示例：id,name;courseName,score,credit|courseName,score,credit|...
    void loadFromFile(const string &filename) {
        ifstream infile(filename.c_str());
        if (!infile) {
            cout << "文件 " << filename << " 打开失败！" << endl;
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
        cout << "数据加载成功！" << endl;
    }
    
    // 保存数据到txt文件
    void saveToFile(const string &filename) {
        ofstream outfile(filename.c_str());
        if (!outfile) {
            cout << "文件 " << filename << " 打开失败！" << endl;
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
        cout << "数据保存成功！" << endl;
    }
};

// 控制器：处理业务逻辑
class StudentController {
private:
    StudentRepository repository;
public:
    // 添加学生
    void addStudent() {
        string id, name;
        cout << "请输入学生ID: ";
        cin >> id;
        cout << "请输入学生姓名: ";
        cin >> name;
        Student student(id, name);
        int courseCount;
        cout << "请输入课程数量: ";
        cin >> courseCount;
        for (int i = 0; i < courseCount; i++){
            string courseName;
            float score, credit;
            cout << "请输入第 " << i + 1 << " 门课程名称: ";
            cin >> courseName;
            cout << "请输入该课程成绩: ";
            cin >> score;
            cout << "请输入该课程学分: ";
            cin >> credit;
            student.addCourse(Course(courseName, score, credit));
        }
        repository.addStudent(student);
        cout << "学生添加成功！" << endl;
        // 预警：存在异常成绩
        if (student.hasAbnormalScore()){
            cout << "注意：该学生存在异常成绩（低于" << SCORE_THRESHOLD << "分）！" << endl;
        }
    }
    
    // 删除学生
    void deleteStudent() {
        string id;
        cout << "请输入要删除的学生ID: ";
        cin >> id;
        if (repository.deleteStudent(id))
            cout << "学生删除成功！" << endl;
        else
            cout << "未找到该学生！" << endl;
    }
    
    // 修改学生信息
    void updateStudent() {
        string id;
        cout << "请输入要修改的学生ID: ";
        cin >> id;
        Student* student = repository.getStudentById(id);
        if (student == NULL){
            cout << "未找到该学生！" << endl;
            return;
        }
        cout << "当前姓名：" << student->name << "，请输入新姓名：";
        cin >> student->name;
        int choice;
        cout << "是否修改课程信息？（1-是，0-否）：";
        cin >> choice;
        if (choice == 1){
            student->courses.clear();
            int courseCount;
            cout << "请输入课程数量: ";
            cin >> courseCount;
            for (int i = 0; i < courseCount; i++){
                string courseName;
                float score, credit;
                cout << "请输入第 " << i + 1 << " 门课程名称: ";
                cin >> courseName;
                cout << "请输入该课程成绩: ";
                cin >> score;
                cout << "请输入该课程学分: ";
                cin >> credit;
                student->addCourse(Course(courseName, score, credit));
            }
        }
        repository.updateStudent(*student);
        cout << "学生信息更新成功！" << endl;
        if (student->hasAbnormalScore()){
            cout << "注意：该学生存在异常成绩（低于" << SCORE_THRESHOLD << "分）！" << endl;
        }
    }
    
    // 查询学生信息
    void queryStudent() {
        string id;
        cout << "请输入要查询的学生ID: ";
        cin >> id;
        Student* student = repository.getStudentById(id);
        if (student == NULL){
            cout << "未找到该学生！" << endl;
            return;
        }
        cout << "学生ID: " << student->id << "\n姓名: " << student->name << endl;
        cout << "课程信息：" << endl;
        for (vector<Course>::const_iterator it = student->courses.begin(); it != student->courses.end(); ++it) {
            cout << "课程名称: " << it->courseName 
                 << ", 成绩: " << it->score 
                 << ", 学分: " << it->credit;
            if (it->score < SCORE_THRESHOLD)
                cout << " [异常成绩]";
            cout << endl;
        }
        cout << fixed << setprecision(2);
        cout << "计算得到的GPA: " << student->calculateGPA() << endl;
    }
    
    // 列出所有学生信息
    void listAllStudents() {
        const map<string, Student>& students = repository.getAllStudents();
        if (students.empty()){
            cout << "暂无学生数据！" << endl;
            return;
        }
        for (map<string, Student>::const_iterator it = students.begin(); it != students.end(); ++it) {
            const Student &student = it->second;
            cout << "---------------------------" << endl;
            cout << "学生ID: " << student.id << "\n姓名: " << student.name << endl;
            cout << "课程信息：" << endl;
            for (vector<Course>::const_iterator jt = student.courses.begin(); jt != student.courses.end(); ++jt) {
                cout << "课程名称: " << jt->courseName 
                     << ", 成绩: " << jt->score 
                     << ", 学分: " << jt->credit;
                if (jt->score < SCORE_THRESHOLD)
                    cout << " [异常成绩]";
                cout << endl;
            }
            cout << "GPA: " << fixed << setprecision(2) << student.calculateGPA() << endl;
        }
        cout << "---------------------------" << endl;
    }
    
    // 保存数据到文件
    void saveData() {
        string filename;
        cout << "请输入保存文件名（例如 students.txt）: ";
        cin >> filename;
        repository.saveToFile(filename);
    }
    
    // 从文件加载数据
    void loadData() {
        string filename;
        cout << "请输入加载文件名（例如 students.txt）: ";
        cin >> filename;
        repository.loadFromFile(filename);
    }
}; // 结束 StudentController 类

// 视图：显示菜单
void showMenu() {
    cout << "\n========== 学生成绩管理终端 ==========" << endl;
    cout << "1. 添加学生" << endl;
    cout << "2. 删除学生" << endl;
    cout << "3. 修改学生信息" << endl;
    cout << "4. 查询学生信息" << endl;
    cout << "5. 列出所有学生" << endl;
    cout << "6. 保存数据" << endl;
    cout << "7. 加载数据" << endl;
    cout << "0. 退出" << endl;
    cout << "请选择操作: ";
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
                cout << "退出系统，再见！" << endl;
                return 0;
            default:
                cout << "无效选项，请重新选择！" << endl;
                break;
        }
    }
    return 0;
}

