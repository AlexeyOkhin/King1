#define signature "EIFORIA 1.3 SAVE FILE"
#define randomize_ext()    srand((unsigned)time(NULL)*getpid())

/* описание возможных ресурсов */
/* и их начальные значения */
long money = 10000;	// деньги, руб
long gold  = 0;		// золото, кг
long land  = 100;	// земля, га
long zerno = 1000;	// зерно, тонн
long krest = 100;	// крестьяне, душ
long guard = 100;	// гвардия, чел

/* текущее состояние ресурсов */
long cur_money, cur_gold, cur_land, cur_zerno, cur_krest, cur_guard;
long build_xram;	// построено храмов
long ochki;		// набранные игровые очки

/* средние цены */
long pr_gold   = 1000;	// руб/кг
long pr_land   =  200;	// руб/га
long pr_zerno  =   10;	// руб/тонн
long pr_krest  =   50;	// руб/душу
long pr_guard  =  100;	// руб/чел
long sod_guard =   10;	// руб/год

/* текущие цены */
long cur_pr_gold;
long cur_pr_land;
long cur_pr_zerno;
long cur_pr_krest;
long cur_pr_guard;

long for_kar;		// выделено денег на караван
long for_xram;		// выделено денег на храм
long abs_sod_guard;	// абсолютная стоимость содержания в текущем году
long run_krest;		// убежало крестьян в текущем году
long add_krest;		// родилось крестьян в текущем году
long run_guard;		// дезертировало гвардейцев в текущем году
long eat_rat;		// сожрали зерна крысы
long grab_money;	// украл денег визирь
long grab_money2;	// украл денег визирь
long grab_gold;		// украдено золота из сокровищницы

/* расход зерна на единицу */
long ed_posev=1;	// на посев на 1 га
long ed_eat=3;		// на еду на 1 душу (krest + guard)

/* расход зерна в текущем году */
long for_posev;		// на посев
long for_eat;		// на еду

long add_zerno;		// абсолютный урожай в текущем году
long urog=7;		// прирост зерна, средний/га
long fl_urog;		// флаг урожайности
long fl_r;		// флаг революции
long fl_kar;		// флаг каравана
long fl_marry;		// флаг женитьбы
long fl_end;		// флаг окончания
long fl_vis;		// флаг визиря
long fl_mar_war;	// флаг войны из-за отказа

long fl_intro = 1;
long fl_vanilla = 0;
long fl_autosave = 1;
char save_path[_MAX_PATH+1];

void intro();
void new_game(long year);
void prn_rule();


func req_zerno ()
{

  var n = (cur_krest + cur_guard) * ed_eat
  if (!fl_vanilla) n = ceil (n * .7)
  n2 = cur_land < cur_krest ? cur_land * ed_posev: cur_krest * ed_posev
  return n + n2
}

func beg_init ()
{
  self.cur_money = self.money
  self.cur_gold  = self.gold
  self.cur_land  = self.land
  self.cur_zerno = self.zerno
  self.cur_krest = self.krest
  self.cur_guard = self.guard
  self.fl_r = 0
  self.fl_mar_war = 0
}

long get_chislo (void) // строку в число
{
  char s[255];
  do
  {
    scanf ("%10s", s);
  }
  while ((atol (s) <= 0) && (s[0] != '0'));
  return atol (s);
}

long check_enter (long v_old, long v_new)  // проверка ввода ( у вас столько нет)
{
  while (v_new > v_old)
  {
    printf ("\nУ Вас столько нет. Повторите ввод: ");
    v_new = get_chislo();
  }
  return v_new;
}

func visir_message ()
{
  char s[255];
  let s =  "Собрано /(add_zerno) тонны"
  print ("\nВаше величество, прибыл главный визирь с докладом.")
  print ("\nВизирь сообщает:")
  print ("\nЖалованье гвардии за прошлый год составило /(abs_sod_guard) рубль")
  switch (fl_urog)
  {
    case 0:
      print ("\nСтрашная засуха поразила посевы. Очень неурожайный год.")
      print ("\n%s зерна. Собрана всего /(add_zerno) тонны")
      break;
    case 1:
      print ("\nУрожайность была низкая. /(s) зерна."
      break;
    case 2:
      printf ("\nСредний по урожайности год.")
      printf ("\nВаши крестьяне собрали /(add_zerno)тонн зерна."
     break;
    case 3:
      printf ("\nУрожайный год. /(s) зерна.")
      break;
    case 4:
      printf ("\nПролившиеся вовремя дожди обеспечили невиданно высокий урожай.");
      printf ("\nАмбары ломятся от зерна - /(s)!")
      break;
  }
  if (eat_rat > 0)
    printf ("\nПреступная халатность! Крысы сожрали %s зерна!", incline_words ("", "", eat_rat, "тонну", "тонны", "тонн"));
  if (add_krest > 0)
    printf ("\nЧисло Ваших подданных увеличилось. %s.", incline_words ("Родился", "Родилось", add_krest, "ребёнок", "ребёнка", "детей"));
  if (run_krest != -1)
    printf("\nВашим крестьянам не хватает земли. %s", incline_words ("Сбежал", "Сбежало", run_krest, "человек.", "человека.", "человек."));
  if (run_guard != -1)
  {
    printf ("\nНе хватило денег на выплату денежного довольствия Вашей гвардии.");
    printf ("\n%s", incline_words ("Дезертировал", "Дезертировало", run_guard, "солдат.", "солдата.", "солдат."));
  }
  if (grab_gold > 0)
    printf("\nСкандал! Из сокровищницы %sкг золота!", incline_words ("похищен", "похищено", grab_gold, "", "", ""));
  if (grab_money > 0)
    printf("\nКража! Визирь похитил %li руб. и скрылся!..", grab_money);
}

func make_price ()
{
  self.cur_pr_gold  = ((self.pr_gold  * 75) / 100) + (random (in: 0...50) * pr_gold  / 100);
  self.cur_pr_land  = ((self.pr_land  * 75) / 100) + (random (in: 0...50) * pr_land  / 100);
  self.cur_pr_zerno = ((self.pr_zerno * 75) / 100) + (random (in: 0...50) * pr_zerno / 100);
  self.cur_pr_krest = ((self.pr_krest * 75) / 100) + (random (in: 0...50) * pr_krest / 100);
  self.cur_pr_guard = ((self.pr_guard * 75) / 100) + (random (in: 0...50) * pr_guard / 100);
}

func prn_birge_helper (char *s, long cur_in, long cur_pr_in, long nominal_pr_in)
{
  char cur[26];
  char cur_pr[26];
  char kupit[26];
  double f;
  format_string (cur_in, cur);
  format_string (cur_pr_in, cur_pr);
  format_string (cur_money / cur_pr_in, kupit);
  f = cur_pr_in * 100 / nominal_pr_in;
  printf ("\n║ %-14s │ %11s  │  %5s  │ %6.1lf%%  │ %11s  ║", s, cur, cur_pr, f, kupit);
}

func prn_birge (long year)
{
  char s[26];
  printf("\n                   Состояние Ваших дел на %i-й год правления.", year);
  format_string (cur_money, s);
  printf ("\nНаличность в казне: %s руб.", s);
  printf ("\n╔════════════════╤══════════════╤═════════╤══════════╤══════════════╗");
  printf ("\n║    Название    │    Запасы    │  Цена   │   Курс   │ Можно купить ║");
  printf ("\n╠════════════════╪══════════════╪═════════╪══════════╪══════════════╣");
  prn_birge_helper ("Золото, кг",      cur_gold,  cur_pr_gold,  pr_gold);
  prn_birge_helper ("Земля, га",       cur_land,  cur_pr_land,  pr_land);
  prn_birge_helper ("Зерно, тонн",     cur_zerno, cur_pr_zerno, pr_zerno);
  prn_birge_helper ("Крестьяне, душ",  cur_krest, cur_pr_krest, pr_krest);
  prn_birge_helper ("Гвардия, чел.",   cur_guard, cur_pr_guard, pr_guard);
  printf ("\n╚════════════════╧══════════════╧═════════╧══════════╧══════════════╝");
  printf ("\nТребуется зерна: %s.", incline_words ("", "", req_zerno(), "тонна", "тонны", "тонн"));
  if (fl_kar) printf ("         Караван прибудет через: %s.", incline_words ("", "", 5-fl_kar, "ход", "хода", "ходов"));
}

void torgovla_helper (char *s1, char *s2, char *s3, long cur_pr, long *cur_item)
{
  long n;
  long cur;
  printf ("\n\nВыберите тип операций с %s: ", s1);
  n = menu (2, "'Покупать' 'Продавать'", 1, 24);
  if (n == 1)
  {
    printf ("\n\nСколько %s %s: ", s2, s3);
    cur = get_chislo();
    if (cur * cur_pr > cur_money)
    {
      printf ("\nУ Вас не хватает денег чтобы оплатить покупку!");
      readkey();
    }
    else
    {
      *cur_item += cur;
      cur_money -= cur * cur_pr;
    }
  }
  else
  {
    printf ("\n\nСколько %s желаете продать: ", s2);
    cur = get_chislo();
    cur = check_enter (*cur_item, cur);
    *cur_item -= cur;
    cur_money += cur * cur_pr;
  }
}

void torgovla (long god)
{
  long nn;
  do
  {
    clrscr();
    prn_birge (god);
    printf ("\n\nВыберите товар, с которым проводить операции:");
    nn = menu (6, "'Золото' 'Земля' 'Зерно' 'Крестьяне' 'Гвардия' 'Выход с биржи'", 1, 16);
    if (nn != 6)
    {
      switch (nn)
      {
        case 1: torgovla_helper ("золотом",     "килограмм золота", "желаете купить", cur_pr_gold,  &cur_gold);  break;
        case 2: torgovla_helper ("землей",      "гектар земли",     "желаете купить", cur_pr_land,  &cur_land);  break;
        case 3: torgovla_helper ("зерном",      "тонн зерна",       "желаете купить", cur_pr_zerno, &cur_zerno); break;
        case 4: torgovla_helper ("крестьянами", "душ крестьян",     "желаете купить", cur_pr_krest, &cur_krest); break;
        case 5: torgovla_helper ("гвардией",    "гвардейцев",       "желаете нанять", cur_pr_guard, &cur_guard); break;
      }
    }
  }
  while (nn != 6);
}

void choice_zerno (void)
{
long n;
long cur;
char s1[255];
char s2[255];
if (fl_vanilla)
{
  printf("\n\nЖелаете сами распределить расход зерна (y/n): ");
  n=get_choice();
}
else n=0;
if (n==0)
 {
  for_eat=(cur_krest+cur_guard)*ed_eat;
  if (!fl_vanilla) for_eat = ceil (for_eat * .7);
  for_posev=min(cur_land,cur_krest)*ed_posev;
  if ((for_eat+for_posev)<=cur_zerno)
   {
    cur_zerno=cur_zerno-for_eat-for_posev;
    strcpy (s1, incline_words ("", "", for_eat, "тонна", "тонны", "тонн"));
    strcpy (s2, incline_words ("", "", for_posev, "тонна", "тонны", "тонн"));
    printf("\n\nВыделено зерна: на еду - %s, на посев - %s.", s1, s2);
    printf("\nОставшийся запас в амбарах: %s.", incline_words ("", "", cur_zerno, "тонна", "тонны", "тонн"));
   }
  else
   {
    printf("\n\nНе могу самостоятельно распределить зерно.");
    printf("\nНе хватает зерна на посев и еду по полной норме.");
    printf("\nПожалуйста, распределите зерно лично.");
    n=1;
   }
 }
if (n==1)
 {
  printf("\nНапоминаю, Ваше состояние сейчас составляет:");
  printf("\nЗемля - %li га, крестьяне - %li душ, гвардия - %li чел.",cur_land,cur_krest,cur_guard);
  printf("\nЗапас зерна в амбарах - %s.", incline_words ("", "", cur_zerno, "тонна", "тонны", "тонн"));
  printf("\n\nУкажите расход зерна на еду: ");
  cur=get_chislo();
  cur=check_enter(cur_zerno,cur);
  for_eat=cur;
  cur_zerno-=cur;
  printf("\nУкажите расход зерна на посев: ");
  cur=get_chislo();
  cur=check_enter(cur_zerno,cur);
  for_posev=cur;
  cur_zerno-=cur;
  printf("\nОставшийся запас в амбарах: %s.", incline_words ("", "", cur_zerno, "тонна", "тонны", "тонн"));
 }
}

/* 0 - страшная засуха, 1 - плохонькая урожайность, 2 - средняя урожайность
   3 - хорошая урожайность, 4 - невиданно плодородный год */

long make_turn (void)
{
long n;
long a,b;
double f,f1,f2,f3;
// обработка (крестьяне+гвардия) - зерно
f1=for_eat;
f2=ed_eat;
f3=cur_krest+cur_guard;
f=(f1/f2)/f3;
printf("\n\nНорма продуктов на год на 1 человека составляет %.1lf%% от необходимой.",f*100);
if (f>=1.) printf("\nНарод доволен таким щедрым правитетем.");
//if ((f<1.)&&(f>=0.7)) printf("\nКормите народ получше, не то получите РЕВОЛЮЦИЮ...");
if ((f<0.7)&&(f>=0.4))
 {
  n=random(100);
  if (n<100-(f*100))
   {
    printf("\nВы доигрались... Народ не стал терпеть такие унижения и сверг Вас!!!");
    printf("\nНе доводите больше свой народ до РЕВОЛЮЦИИ!!!");
    return 1;
   }
  else
   {
    printf("\nНедовольство вами резко возросло. Вы сильно рискуете...");
    printf("\nТолько случай спас Вас в этот раз от РЕВОЛЮЦИИ...");
   }
 }
if (f<0.4)
 {
  printf("\nДа Вы что, издеваетесь?!! Так морить голодом свой народ!..");
  printf("\nПродали бы лишних крестьян, изверг, если прокормить не можете...");
  printf("\nЕстественно, умирающий от голода народ сверг такого тирана...");
  printf("\nПолучите РЕВОЛЮЦИЮ!!!");
  return 1;
 }
// обработка урожая
fl_urog=random(5);
a=min(cur_krest,cur_land);
b=min(a,for_posev);
add_zerno=(fl_urog*2+3)*b;
cur_zerno+=add_zerno;
// обработка крыс
n=random(100);
if (n<20)
 {
  eat_rat=(random(20)*cur_zerno)/100;
  cur_zerno-=eat_rat;
 }
else eat_rat=-1;
// обработка земля - крестьяне
if (cur_krest>cur_land)
 {
  run_krest=random(cur_krest-cur_land);
  cur_krest-=run_krest;
 }
else run_krest=-1;
n=random(10)+6;
add_krest=(cur_krest*n)/100;
cur_krest+=add_krest;
// обработка гвардия - деньги
abs_sod_guard=(cur_guard+1)*sod_guard;
if (abs_sod_guard>cur_money)
 {
  n=random(10)+6;
  run_guard=(cur_guard*n)/100;
  cur_guard-=run_guard;
  abs_sod_guard=(cur_guard+1)*sod_guard;
  if (abs_sod_guard>cur_money) abs_sod_guard=cur_money;
 }
else
 run_guard=-1;
cur_money-=abs_sod_guard;
// обработка похищения золота
if (cur_gold>0)
 {
  n=random(100);
  if (n<20)
   {
    grab_gold=(random(25)*cur_gold)/100;
    cur_gold-=grab_gold;
   }
  else grab_gold=-1;
 }
else grab_gold=-1;
// обработка визирь - деньги
if (cur_money>0)
 {
  n=random(100);
  if (n<10)
   {
    grab_money=(random(25)*cur_money)/100;
    grab_money2=grab_money;
    cur_money-=grab_money;
    fl_vis=1;
   }
  else grab_money=-1;
 }
else grab_money=-1;
return 0;
}

void prn_result_helper (char *s, long i, char *s2, double modifier)
{
  long cur;
  cur = i / modifier;
  printf ("║ %-16s │ %12li │ %13s │ %6li ║\n", s, i, s2, cur);
  ochki += cur;
}

void prn_result (long year)
{
  ochki = 0;
  clrscr();
  if (!fl_r)
    printf ("Ваше правление завершилось...\n\n");
  else
  {
    printf("Голодающий народ ВОССТАЛ и свергнул нерадивого правителя!!!\n");
    printf("Поздравляю Вас, батенька, с РЕВОЛЮЦИЕЙ, ха-ха...\n\n");
  }
  printf ("                   Состояние Ваших дел после %s правления.\n\n", incline_words ("", "", year, "года", "лет", "лет"));
  printf ("За Ваше состояние Вам дается следующее количество очков:\n");
  printf ("╔══════════════════╤══════════════╤═══════════════╤════════╗\n");
  printf ("║     Название     │   Значение   │  Модификатор  │  Очки  ║\n");
  printf ("╠══════════════════╪══════════════╪═══════════════╪════════╣\n");
  prn_result_helper ("Деньги",          cur_money,  "/ 1000",  1000);
  prn_result_helper ("Золото",          cur_gold,      "* 2",    .5);
  prn_result_helper ("Земля",           cur_land,      "/ 5",     5);
  prn_result_helper ("Зерно",           cur_zerno,   "/ 100",   100);
  prn_result_helper ("Крестьяне",       cur_krest,    "/ 20",    20);
  prn_result_helper ("Гвардия",         cur_guard,    "/ 10",    10);
  prn_result_helper ("Новые храмы",     build_xram,  "* 200", 0.005);
  prn_result_helper ("Время правления", year,         "* 10",    .1);
  printf ("╚══════════════════╧══════════════╧═══════════════╧════════╝\n");
  printf("Общая сумма Ваших очков: %li\n\n", ochki);
  printf("Ну что ж... Поздравляю с успешным (?) окончанием игры.\n");
  printf ("P.S. ");
  if (ochki<=100)
    printf ("Вам бы лучше гусей пасти... Вместо Ваших крестьян...");
  else if (ochki <= 300)
    printf ("Для новичка - сойдет... Хотя, конечно, неважно...");
  else if (ochki <= 500)
    printf ("Ну, это уже кое-что... Худо-бедно, да ладно...");
  else if (ochki <= 1000)
    printf ("Ну вот, кое-что уже получается. Старайтесь дальше...");
  else if (ochki <= 3000)
    printf ("Неплохо, весьма неплохо... Уважаю...");
  else if (ochki <= 5000)
    printf ("Что ж, видно, играть Вы умеете... Весьма недурственно...");
  else if (ochki <= 10000)
    printf ("Круто, что говорить... Да Вы, батенька, профессионал...");
  else if (ochki <= 100000L)
    printf ("Прости их, господи... Ну Вы, блин, даете!!!");
  else
    printf ("NO pity, NO mercy, NO REGRET!!!!!!!!!!");
  printf ("\n");
  readkey();
}

long snarad_kar (void)
{
  long n;
  long cur;
  printf ("\n\nЗаморский купец предлагает снарядить караван. Вы согласны (y/n): ");
  n = get_choice();
  if (n == 0) return 0;
  printf ("\nВ казне - %li руб, сколько на караван: ", cur_money);
  cur = get_chislo();
  cur = check_enter (cur_money, cur);
  printf ("Караван отправился за тридевять земель...");
  cur_money -= cur;
  for_kar = cur;
  return 1;
}

void grabeg_kar (void)
{
  long n, grab;
  n = random (100);
  if (n < 5)
  {
    printf("\n\nПроизошло ЧП! Ваш караван полностью разграблен бандитами!!!");
    fl_kar = 0;
    for_kar = 0;
  }
  else
  {
    n = random (40);
    grab = (for_kar * n) / 100;
    printf ("\n\nВнимание, ЧП! Ваш караван ограблен бандитами на сумму %li руб.!!!", grab);
    for_kar -= grab;
  }
}

void pribil_kar (void)
{
  long prib;
  prib = for_kar * 6;
  printf ("\n\nВернулся Ваш караван! Получено прибыли на сумму %li руб.!", prib);
  cur_money += prib;
  fl_kar = 0;
  for_kar = 0;
}

void mitropolit (void)
{
  long cur, nn;
  double f, f1, f2;
  printf ("\n\nМитрополит Вашего государства просит денег на новый храм.");
  printf ("\nСколько выделяте (в казне %li руб.): ", cur_money);
  cur = get_chislo();
  cur = check_enter (cur_money, cur);
  for_xram += cur;
  f1 = cur; f2 = cur_money;
  f = f1/f2; f *= 100;
  cur_money -= cur;
  if (f <= 1)
    printf("\nТы что, насмехаешься?! Скряга!!! За твою жадность ты сгоришь в аду!");
  else if (f <= 10)
    printf("\nОпомнись, сын мой! Нельзя же быть таким жадным, это смертный грех!");
  else if (f <= 20)
    printf("\nНе слишком-то щедры твои приношения, сын мой. Можно было дать и побольше.");
  else if (f <= 30)
    printf("\nЧто ж, спасибо и на этом. Ваши приношения пойдут на богоугодное дело.");
  else if (f <= 50)
    printf("\nБлагодарю тебя, сын мой. Твоя щедрость будет оценена по достоинству.");
  else
    printf("\nВо всех храмах страны поют молитвы во славу мудрого и щедрого короля!");
  nn = for_xram / 100000L;
  if (nn > 0)
  {
    for_xram -= nn * 100000L;
    build_xram += nn;
    printf ("\n%s.", incline_words ("Построен", "Построено", nn, "новый храм", "новых храма", "новых храмов"));
  }
}

void war_nasledstvo_helper (char *s, long fl_soldiers, long modifier)
{
  long cur;
  long n;
  printf("%s", s);
  n=random(90)+10; cur=cur_money*n/100; cur_money+=cur*modifier;
  printf("\nДеньги - %li руб.",cur);
  n=random(90)+10; cur=cur_gold*n/100; cur_gold+=cur*modifier;
  printf("\nЗолото - %li кг.",cur);
  n=random(90)+10; cur=cur_land*n/100; cur_land+=cur*modifier;
  printf("\nЗемля - %li га.",cur);
  n=random(90)+10; cur=cur_zerno*n/100; cur_zerno+=cur*modifier;
  printf("\nЗерно - %li тонн.",cur);
  n=random(90)+10; cur=cur_krest*n/100; cur_krest+=cur*modifier;
  printf("\nКрестьяне - %li душ.",cur);
  if (fl_soldiers)
  {
    n=random(90)+10; cur=cur_guard*n/100; cur_guard+=cur*modifier;
    printf("\nСолдаты - %li чел.",cur);
  }
}

void nasledstvo (void)
{
  war_nasledstvo_helper ("\n\nУмер Ваш дальний родственник. Вы получили наследство в размере:", 1, 1);
}

void poimali_visir (void)
{
  long cur;
  printf ("\n\nВаша полиция поймала сбежавшего визиря!");
  printf ("\nУ него конфисковано все имущество, а его самого посадили на кол!");
  cur = (random (50) + 50 ) * grab_money2 / 100;
  printf ("\nВ казну возвращено %li руб.", cur);
  cur_money += cur;
  fl_vis = 0;
}

void sosed_marry (void)
{
long n;
long cur;
long prid_money,prid_gold,prid_land,prid_zerno,prid_krest,prid_guard;
printf("\n\nСоседний король сватает за Вас свою дочку.");
printf("\nВ приданое он предлагает:");
n=random(90)+10; cur=cur_money*n/100; prid_money=cur;
printf("\nДеньги - %li руб.",cur);
n=random(90)+10; cur=cur_gold*n/100; prid_gold=cur;
printf("\nЗолото - %li кг.",cur);
n=random(90)+10; cur=cur_land*n/100; prid_land=cur;
printf("\nЗемля - %li га.",cur);
n=random(90)+10; cur=cur_zerno*n/100; prid_zerno=cur;
printf("\nЗерно - %li тонн.",cur);
n=random(90)+10; cur=cur_krest*n/100; prid_krest=cur;
printf("\nКрестьяне - %li душ.",cur);
n=random(90)+10; cur=cur_guard*n/100; prid_guard=cur;
printf("\nСолдаты - %li чел.",cur);
printf("\n\nВы согласны (y/n): ");
n=get_choice();
if (n==1)
 {
  cur=(random(40)+20)*cur_money/100;
  printf("\n\nПоздравляю. На свадебный пир потрачено %li руб.",cur);
  cur_money-=cur;
  cur_money+=prid_money; cur_gold+=prid_gold; cur_land+=prid_land;
  cur_zerno+=prid_zerno; cur_krest+=prid_krest; cur_guard+=prid_guard;
  fl_marry=1;
 }
else
 {
  printf("\n\nАх так?!! Вы пренебрегаете моим предложением??! Готовьтесь к ВОЙНЕ!");
  fl_mar_war=1;
 }
}

void begin_war (void)
{
long cur,ras;
long your_men,enemy_men;
long n,victory;
cur=random((cur_guard+cur_krest)*2);
enemy_men=cur;
your_men=cur_guard;
ras=cur-(50*cur/100)+random(cur);
printf("\nРазведка доносит о предполагаемой численности войск врага: %s.", incline_words ("", "", ras, "солдат", "солдата", "солдат"));
printf("\nВаши силы: %s. Объявляете мобилизацию крестьян (y/n)? ", incline_words ("", "", your_men, "солдат", "солдата", "солдат"));
n=get_choice();
if (n==1)
 {
  cur=(random(50)+50)*cur_krest/100;
  printf("\n%s.", incline_words ("Мобилизован", "Мобилизовано", cur, "человек", "человека", "человек"));
  your_men+=cur;
 }
printf("\nЕсть возможность завербовать наемников на время этой войны.");
printf("\nОдин наемник стоит 100 рублей. Будете вербовать (y/n)? ");
n=get_choice();
if (n==1)
 {
  printf("\nСколько наемников хотите нанять (в казне - %li руб.): ",cur_money);
  cur=get_chislo();
  cur=check_enter(cur_money/100,cur);
  your_men+=cur;
  cur_money-=cur*100;
 }
else printf("\n");
printf("\nПеред битвой выяснилось точное число войск противника: %s.", incline_words ("", "", enemy_men, "солдат", "солдата", "солдат"));
printf("\nВаши войска составляют %s.", incline_words ("", "", your_men, "солдат", "солдата", "солдат"));
printf("\nНажмите любую клавишу для начала сражения...");
readkey();
victory=0;
n=random(enemy_men+your_men*2);
if (n<=your_men*2) victory=1;
  if (victory) war_nasledstvo_helper ("\n\nВы победили! Ваша армия захватила трофеи:", 0, 1);
  else war_nasledstvo_helper ("\n\nВы проиграли... Ваши потери в этой войне:", 1, -1);
}

void rodilsa_sin (void)
{
long cur;
printf("\n\nУ Вас родился сын! Поздравляю! Ваша династия не угаснет в веках!");
cur=(random(40)+20)*cur_money/100;
printf("\nНа праздничный банкет по случаю рождения сына потрачено %li руб.");
cur_money-=cur;
}

void dead_wife (void)
{
  long cur;
  long n;
  printf ("\n");
  if (fl_vanilla)
  {
    printf ("\nПрибыл гонец от королевы. Впустить (y/n)? ");
    n = get_choice();
    if (n == 0)
      printf("\nХоть Вы и не приняли гонца, но печальная весть все равно дошла до Вас.");
  }
  printf ("\nВеликое несчастье! Умерла королева! Овдовевший монарх безутешен!");
  cur = (random (40) + 20) * cur_money / 100;
  printf ("\nНа похороны королевы потрачено %li руб.", cur);
  cur_money -= cur;
  fl_marry = 0;
}

void koroleva_prosit (void)
{
long n,cur;
n=random(100);
if (n<15)
 {
  printf("\n\nПрибыл гонец от королевы. Впустить (y/n)? ");
  n=get_choice();
  if (n==0) return;
  cur=(random(30)+1)*cur_money/100;
  printf("\nКоролева просит %li руб. на новое платье. Выделить средства (y/n)? ",cur);
  n=get_choice();
  if (n==1)
   {
    printf("\nКоролева благодарит Вас /лично и ОЧЕНЬ горячо... :-) /");
    cur_money-=cur;
   }
  else
   printf("\nКоролева ЖУТКО на Вас обиделась... Видеть Вас больше не желает...");
  printf("\n\nНажмите любую клавишу...");
  readkey();
 }
else
 {
  n=random(100);
  if (n<15)
   {
    printf("\n\nПрибыл гонец от королевы. Впустить (y/n)? ");
    n=get_choice();
    if (n==0) return;
    cur=(random(30)+1)*cur_money/100;
    printf("\n\nКоролева просит %li руб., чтобы устроить бал. Выделить средства (y/n)? ",cur);
    n=get_choice();
    if (n==1)
     {
      printf("\nКоролева благодарит Вас /лично и ОЧЕНЬ горячо... :-) /");
      cur_money-=cur;
     }
    else
     printf("\nКоролева на Вас обиделась... Хоть и не очень сильно, но все таки...");
    printf("\n\nНажмите любую клавишу...");
    readkey();
   }
 }
}

long shaman (void)
{
long cur,n,n2,fl;
printf("\nМестный шаман берется вылечить Вас (с вероятностью 20%%...)");
cur=(random(40)+1)*cur_money/100;
printf("\nНо за это он требует половину Вашего золота и %li руб.",cur);
printf("\nВы согласны (y/n)? ");
n=get_choice();
if (n==1)
 {
  cur_money-=cur;
  cur_gold-=cur_gold/2;
  n2=random(100);
  if (n2<20)
   {
    printf("\n\nКолдовство шамана помогло! Вы выздоровели!");
    fl=0;
   }
  else
   {
    printf("\n\nШаман ничем не смог помочь Вам... Король УМЕР!!!");
    fl=1;
   }
 }
else
 {
  n2=random(100);
  if (n2<5)
   {
    printf("\n\nСлучилось ЧУДО! Вы победили болезнь и встали со смертного одра!");
    fl=0;
   }
  else
   {
    printf("\n\nБолезнь победила Вас... Король УМЕР!!!");
    fl=1;
   }
 }
return fl;
}

long korol_bolen(long i)
{
long fl,n,n2,cur;
cur=(random(30)+1)*cur_money/100;
printf("\n\nВы заболели! За лечение лекарь просит %s.", incline_words ("", "", cur, "рубль", "рубля", "рублей"));
printf("\nВы можете выздороветь сами, но можете и не выздороветь...");
printf("\nБудете лечиться (y/n)? ");
n=get_choice();
if (n==1)
 {
  cur_money-=cur;
  n2=random(100);
  if (n2<5)
   {
    printf("\nЛечение не дало результатов. Вы при смерти...");
    fl=shaman();
   }
  else
   {
    printf("\nЛечение помогло, Вы благополучно выздоровели...");
    fl=0;
   }
 }
else
 {
  n2=random(100);
  if (n2<i*2)
   {
    printf("\nЯ же Вас предупреждал! Болезнь обострилась, Вы при смерти!");
    fl=shaman();
   }
  else
   {
    printf("\nВы благополучно исцелились сами...");
    fl=0;
   }
 }
return fl;
}

void savegame (long year)
{
  char filename[_MAX_PATH+1];
  FILE *fp;

  sprintf (filename, "%s%c%03li.SAV", save_path, path_separator, year);
  fp = fopen (filename, "w");
  if (fp == NULL)
  {
    perror ("\n\nОШИБКА");
    printf ("Не могу создать файл автосохранения: %s\n", filename);
    printf ("АВТОСОХРАНЕНИЕ ОТКЛЮЧЕНО.\n");
    printf ("Нажмите любую клавишу для продолжения...");
    fl_autosave = 0;
    readkey();
    return;
  }
  fprintf (fp, "%s\n", signature);
  fprintf (fp, "%li\n", cur_money);
  fprintf (fp, "%li\n", cur_gold);
  fprintf (fp, "%li\n", cur_land);
  fprintf (fp, "%li\n", cur_zerno);
  fprintf (fp, "%li\n", cur_krest);
  fprintf (fp, "%li\n", cur_guard);
  fprintf (fp, "%li\n", fl_r);
  fprintf (fp, "%li\n", fl_mar_war);

  fprintf (fp, "%li\n", fl_kar);
  fprintf (fp, "%li\n", for_kar);
  fprintf (fp, "%li\n", fl_marry);
  fprintf (fp, "%li\n", fl_end);
  fprintf (fp, "%li\n", fl_vis);
  fprintf (fp, "%li\n", build_xram);
  fprintf (fp, "%li\n", for_xram);
  fprintf (fp, "%li\n", year);

  fprintf (fp, "%li\n", abs_sod_guard);
  fprintf (fp, "%li\n", fl_urog);
  fprintf (fp, "%li\n", add_zerno);
  fprintf (fp, "%li\n", eat_rat);
  fprintf (fp, "%li\n", add_krest);
  fprintf (fp, "%li\n", run_krest);
  fprintf (fp, "%li\n", run_guard);
  fprintf (fp, "%li\n", grab_gold);
  fprintf (fp, "%li\n", grab_money);
  fprintf (fp, "%li\n", grab_money2);

  fprintf (fp, "%li\n", cur_pr_gold);
  fprintf (fp, "%li\n", cur_pr_land);
  fprintf (fp, "%li\n", cur_pr_zerno);
  fprintf (fp, "%li\n", cur_pr_krest);
  fprintf (fp, "%li\n", cur_pr_guard);
  fclose (fp);
}

void new_game (long year)
{
long i,n;
long cur;
i = year;
if (i == 1)
{
  if (fl_autosave)
    make_tree();
  beg_init();
  fl_kar=0; fl_marry=0; fl_end=0; fl_vis=0; build_xram=0; for_xram=0;
  make_price();
}

do
 {
  clrscr();
  prn_birge (i);
  if (i>1) visir_message();

  if (fl_kar==5)
   {
    pribil_kar();
    fl_kar=0;
   }

  n=random(100);
  if (n<25)
   {
    printf("\n\nМеждународный кризис! Торговля невозможна!");
    printf("\nВашему государству объявлена экономическая блокада!");
//    printf("\n\nНажмите любую клавишу...");
//    ch=getch(); if (ch==0) ch=getch();
   }
  else
  {
   printf("\n\nЖелаете торговать на бирже (y/n)? ");
   n=get_choice();
   if (n==1) torgovla(i);
  }

  if (fl_kar==0)
   {
    n=random(100);
    if (n<25) fl_kar=snarad_kar();
   }

  if (fl_kar>1)
   {
    n=random(100);
    if (n<20) grabeg_kar();
   }

  if (fl_kar>0) fl_kar++;

  n=random(100);
  if (n<20)
   {
    mitropolit();
//    printf("\n\nНажмите любую клавишу...");
//    ch=getch(); if (ch==0) ch=getch();
   }

  if (fl_mar_war)
   {
    printf("\n\nРазозленный отказом жениться на его дочке, соседний король начал ВОЙНУ!");
    begin_war();
    printf("\n\nНажмите любую клавишу...");
    readkey();
    fl_mar_war=0;
   }

  if (cur_guard<100)
   {
    n=random(100);
    if (n>cur_guard)
     {
      printf("\n\nСоседние короли, видя малочисленность Ваших войск, объявили Вам ВОЙНУ!");
      begin_war();
      printf("\n\nНажмите любую клавишу...");
      readkey();
     }
   }
  else
   {
    n=random(100);
    if (n<30)
     {
      printf("\n\nЕсть возможность объявить войну одному из соседей. Объявляете? ");
      n=get_choice();
      if (n==1)
       {
        begin_war();
        printf("\n\nНажмите любую клавишу...");
        readkey();
       }
     }
   }

  if (fl_vis==1)
   {
    n=random(100);
    if (n<15)
     {
      poimali_visir();
//      printf("\n\nНажмите любую клавишу...");
//      ch=getch(); if (ch==0) ch=getch();
     }
   }

  n=random(100);
  if (n<10)
   {
    nasledstvo();
//    printf("\n\nНажмите любую клавишу...");
//    ch=getch(); if (ch==0) ch=getch();
   }

  if (fl_marry==1)
   {
    n=random(100);
    if (n<10)
     {
      rodilsa_sin();
//      printf("\n\nНажмите любую клавишу...");
//      ch=getch(); if (ch==0) ch=getch();
     }
   }

  if (fl_marry==0)
   {
    n=random(100);
    if (n<15)
     {
      sosed_marry();
//      printf("\n\nНажмите любую клавишу...");
//      ch=getch(); if (ch==0) ch=getch();
     }
   }

  if (fl_marry==1)
   {
    n=random(100);
    if (n<10)
     {
      dead_wife();
//      printf("\n\nНажмите любую клавишу...");
//      ch=getch(); if (ch==0) ch=getch();
     }
   }

  if (fl_marry == 1 && fl_vanilla) koroleva_prosit();

  n=random(100);
  if (n<i*2)
   {
    fl_end=korol_bolen(i);
    printf("\n\nНажмите любую клавишу...");
    readkey();
    if (fl_end) break;
   }

  choice_zerno();

  fl_r=make_turn();
  if (fl_r==1)
   {
    readkey();
//    fl_end=1;
    break;
   }

  make_price();
  if (fl_autosave && i < 50) savegame (i+1);

  printf("\n\nБудете править в следующем году (y/n)? ");
  n=get_choice();
  if (n==0)
  {
//    fl_end=1;
    break;
  }

  if (fl_vanilla)
  {
    printf("\n\nБудете устраивать новогодний бал (y/n)? ");
    n=get_choice();
     if (n==1)
     {
      printf("\nСколько даете на бал (в казне - %li руб.): ",cur_money);
      cur=get_chislo();
      cur=check_enter(cur_money,cur);
      cur_money-=cur;
      if (cur==0)
       {
        printf("Я не понял, Вы устраиваете бал или нет?! Что это за сумма такая - 0 рублей?!");
        printf("\nЕсли денег жалко, то скажите 'НЕТ' сразу и не пудрите мне мозги!!!");
       }
      else
       {
        if (fl_marry>0) printf("Королева благодарит Вас.");
        else printf("Придворные благодарят Вас.");
       }
     }
  }

  i++;
//  printf("\n\nНажмите любую клавишу...");
//  ch=getch(); if (ch==0) ch=getch();
 }
while (i<=50);

if (i>50) i=50;
prn_result (i);
}
