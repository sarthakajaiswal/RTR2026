#include <stdio.h> 

enum BloodGroup 
{
    O_POSITIVE = 0, 
    O_NEGATIVE, 
    A_POSITIVE, 
    A_NEGATIVE, 
    B_POSITIVE, 
    B_NEGATIVE, 
    AB_POSITIVE, 
    AB_NEGATIVE 
}; 

enum Gender 
{
    MALE = 0, 
    FEMALE, 
    TRANSGENDER 
}; 

struct Patient 
{
    unsigned short  id; 
    unsigned short  age; 
    unsigned long long mobile_number; 
    float           weight; 
    enum BloodGroup blood_group; 
    enum Gender     gender;
}; 

void create_patient_entry(unsigned short id, unsigned short age, unsigned long long mobile_number, float weight, enum BloodGroup blood_group, enum Gender gender);
void show_entries(void);  
unsigned short createID(void); 

struct Patient patient_entries[10]; 

int main(void) 
{
    create_patient_entry(createID(), 23, 9999999988, 60.0f, O_POSITIVE, MALE); 
    create_patient_entry(createID(), 20, 8888080808, 30.0f, B_POSITIVE, MALE); 
    create_patient_entry(createID(), 99, 1234567890, 70.0f, AB_POSITIVE, FEMALE); 

    show_entries(); 

    return (0); 
} 

void create_patient_entry(
    unsigned short _id, 
    unsigned short _age, 
    unsigned long long _mobile_number, 
    float _weight, 
    enum BloodGroup _blood_group, 
    enum Gender _gender
)
{
    static int index = 0;  

    patient_entries[index].id = _id; 
    patient_entries[index].age = _age; 
    patient_entries[index].mobile_number = _mobile_number; 
    patient_entries[index].weight = _weight; 
    patient_entries[index].blood_group = _blood_group; 
    patient_entries[index].gender = _gender; 

    index += 1; 
} 

void show_entries(void) 
{
    for(int i = 0; i < sizeof(patient_entries)/sizeof(patient_entries[0]); ++i) 
    {
        if(patient_entries[i].weight <= 0.1f)
            return;  

        printf("ID = %hu\nage = %hu Years\nmobile number = %llu\nWeight = %.2f kg\n", 
            patient_entries[i].id, 
            patient_entries[i].age, 
            patient_entries[i].mobile_number, 
            patient_entries[i].weight 
        ); 

        switch(patient_entries[i].blood_group) 
        {
            case O_POSITIVE: 
                printf("Blood Group = O+\n"); 
                break; 

            case O_NEGATIVE: 
                printf("Blood Group = O-\n"); 
                break; 

            case A_POSITIVE: 
                printf("Blood Group = A+\n"); 
                break; 

            case A_NEGATIVE: 
                printf("Blood Group = A-\n"); 
                break;

            case B_POSITIVE: 
                printf("Blood Group = B+\n"); 
                break; 

            case B_NEGATIVE: 
                printf("Blood Group = B-\n"); 
                break; 

            case AB_POSITIVE: 
                printf("Blood Group = AB+\n"); 
                break; 

            case AB_NEGATIVE: 
                printf("Blood Group = AB-\n"); 
                break; 

            default: 
                printf("Blood Group = -\n");
                break;  
        } 

        switch(patient_entries[i].gender) 
        {
            case MALE: 
                printf("Gender = M\n"); 
                break; 

            case FEMALE: 
                printf("Gender = F\n"); 
                break; 

            case TRANSGENDER: 
                printf("Gender = Trans\n"); 
                break; 
        } 

        printf("-------------------\n"); 
    } 
} 

unsigned short createID(void) 
{
    static unsigned short id = 1;
    
    unsigned short ret = id; 
    id += 1; 

    return (ret); 
} 

