//학생의 Action의 return 값은 
//return Wait(hp, name);
//return Heal(hp, name);
//둘중 하나여야함

int JINaction(int hp, char* name)
{
    int r = rand() % 2; // 0 또는 1
    if (r == 0) {
        return Wait(hp, name);
    }
    else {
        return Heal(hp, name);

    }
}