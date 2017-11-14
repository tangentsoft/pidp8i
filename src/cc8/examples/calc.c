int main()
{
  int x, y, ans;
  int choice;
  int bfr[10];

  ans = 'Y';

  while (1)
  {
    if (ans == 'Y' || ans == 'y')
    {
      printf ("\r\nENTER ANY TWO NUMBERS.\r\n");

      printf ("  ENTER THE FIRST NUMBER: ");
      gets (bfr);
      sscanf (bfr, "%d", &x);

      printf ("  ENTER THE SECOND NUMBER: ");
      gets (bfr);
      sscanf (bfr, "%d", &y);

      printf ("SELECT THE OPERATION:\r\n");

      printf ("1: ADDITION\r\n");
      printf ("2: SUBTRACTION\r\n");
      printf ("3: MULTIPLICATION\r\n");
      printf ("4: DIVISION\r\n");

      printf ("CHOICE:");
      gets (bfr);
      sscanf (bfr, "%d", &choice);

      if (choice == 1) printf ("Result: %d\r\n", x + y);
      if (choice == 2) printf ("Result: %d\r\n", x - y);
      if (choice == 3) printf ("Result: %d\r\n", x * y);
      if (choice == 4) printf ("Result: %d\r\n", x / y);

      printf ("DO YOU WISH TO CONTINUE (Y/N): ");
      ans = getc ();
    }
    else
    {
      break;
    }
  }
}
