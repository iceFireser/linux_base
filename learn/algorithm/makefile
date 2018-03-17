object = sort chart citymap

object_sort_o = sort.o
object_chart_o = chart.o
object_citymap_o = citymap.o

object_sort_c = sort.c
object_chart_c = chart.c
object_citymap_c = citymap.c

all : $(object)
.PHONY : clean

sort : $(object_sort_o)
	gcc -o sort $(object_sort_o)

chart : $(object_chart_o)
	gcc -o chart $(object_chart_o)

citymap : $(object_citymap_o)
	gcc -o $(object_citymap_o)

sort.o : $(object_sort_c)
	gcc -c $(object_sort_c)

chart.o : $(object_chart_c)
	gcc -c $(object_chart_c)

citymap.o : $(object_citymap_c)
	gcc -c $(object_citymap_c)

clean :
	rm $(object) *.o
