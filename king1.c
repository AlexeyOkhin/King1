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
