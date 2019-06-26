#include "sierrachart.h"
SCDLLName("WMO Market Processing Analyzer") 

SCSFExport scsf_WMO_market_processing_analyzer_absolute(SCStudyInterfaceRef sc)
{
	SCInputRef velikost_marketu = sc.Input[6];
	SCInputRef velikost_limitu = sc.Input[7];
	SCInputRef pocet_limitu = sc.Input[8];
	SCInputRef zacatek_rth = sc.Input[11];
	SCInputRef konec_rth = sc.Input[12];
	SCInputRef smer_limitu = sc.Input[13];

	SCInputRef MaximumPositionAllowed = sc.Input[30];
	SCInputRef PositionQuantity = sc.Input[31];
	SCInputRef AllowEntryWithWorkingOrders = sc.Input[32];
	SCInputRef SupportReversals = sc.Input[33];
	SCInputRef SendOrdersToTradeService = sc.Input[34];
	SCInputRef AllowMultipleEntriesInSameDirection = sc.Input[35];
	SCInputRef AllowOppositeEntryWithOpposingPositionOrOrders = sc.Input[36];
	SCInputRef CancelAllOrdersOnEntriesAndReversals = sc.Input[38];
	SCInputRef CancelAllWorkingOrdersOnExit = sc.Input[39];
	SCInputRef AllowOnlyOneTradePerBar = sc.Input[40];

	SCInputRef long_short = sc.Input[43];
		
	if (sc.SetDefaults)
	{
		sc.GraphName = "WMO Market Processing Analyzer - Absolute";
		
		//nastaveni prahovych hodnot 
		velikost_marketu.Name = "Minimal Market Size >= ....";
		velikost_marketu.SetInt(100);

		velikost_limitu.Name = "Mininimal Max Limit >= ....";
		velikost_limitu.SetInt(0);

		pocet_limitu.Name = "Minimal Limit Order Count >= ....";
		pocet_limitu.SetInt(0);

		//nastaveni casu
		zacatek_rth.Name = "Trade From:";
		zacatek_rth.SetTime(HMS_TIME(8, 30, 0));
	
		konec_rth.Name = "Trade Till:";
		konec_rth.SetTime(HMS_TIME(14, 55, 0));

		//nastaveni objemu 
		PositionQuantity.Name = "Position Quantity";
		PositionQuantity.SetInt(6);

		MaximumPositionAllowed.Name = "Maximum Position Allowed";
		MaximumPositionAllowed.SetInt(6);

		//nastaveni boolu 
		smer_limitu.Name = "Limit Direction (Yes), Market Direction (No) ";
		smer_limitu.SetYesNo(true);

		AllowEntryWithWorkingOrders.Name = "Allow Entry With Working Orders (Yes/No)?";
		AllowEntryWithWorkingOrders.SetYesNo(false);

		SupportReversals.Name = "Support Reversals (Yes/No)?";
		SupportReversals.SetYesNo(false);

		SendOrdersToTradeService.Name = "Send Orders To Trade Service (Yes/No)?";
		SendOrdersToTradeService.SetYesNo(false);

		AllowMultipleEntriesInSameDirection.Name = "Allow Multiple Entries In Same Direction (Yes/No)";
		AllowMultipleEntriesInSameDirection.SetYesNo(false);

		AllowOppositeEntryWithOpposingPositionOrOrders.Name = "Allow Opposite Entry With Opposing Position Or Orders (Yes/No)";
		AllowOppositeEntryWithOpposingPositionOrOrders.SetYesNo(false);

		CancelAllOrdersOnEntriesAndReversals.Name = "Cancel All Orders On Entries And Reversals (Yes/No)";
		CancelAllOrdersOnEntriesAndReversals.SetYesNo(false);

		CancelAllWorkingOrdersOnExit.Name = "Cancel All Working Orders On Exit (Yes/No)";
		CancelAllWorkingOrdersOnExit.SetYesNo(true);

		AllowOnlyOneTradePerBar.Name = "Allow Only One Trade Per Bar (Yes/No)";
		AllowOnlyOneTradePerBar.SetYesNo(false);

		// vyber long/short smer
		long_short.Name = "Direction";
		long_short.SetCustomInputStrings("Both;Long;Short");
		long_short.SetCustomInputIndex(0);

		sc.AutoLoop = 1;
		sc.UpdateAlways = 1;
		sc.FreeDLL = 0;

		sc.CalculationPrecedence = LOW_PREC_LEVEL;
		return;
	}

	sc.MaximumPositionAllowed = MaximumPositionAllowed.GetInt(); //ok
	sc.AllowEntryWithWorkingOrders = AllowEntryWithWorkingOrders.GetInt(); //ok
	sc.SupportReversals = SupportReversals.GetYesNo(); //ok
	sc.SendOrdersToTradeService = SendOrdersToTradeService.GetYesNo(); //ok
	sc.AllowMultipleEntriesInSameDirection = AllowMultipleEntriesInSameDirection.GetYesNo(); //ok
	sc.AllowOppositeEntryWithOpposingPositionOrOrders = AllowOppositeEntryWithOpposingPositionOrOrders.GetYesNo(); //ok
	sc.SupportAttachedOrdersForTrading = true; //ok
	sc.CancelAllOrdersOnEntriesAndReversals = CancelAllOrdersOnEntriesAndReversals.GetYesNo(); //ok;
	sc.CancelAllWorkingOrdersOnExit = CancelAllWorkingOrdersOnExit.GetYesNo(); //ok
	sc.AllowOnlyOneTradePerBar = AllowOnlyOneTradePerBar.GetYesNo(); //ok
	sc.MaintainTradeStatisticsAndTradesData = true;

/*	sc.AskVolume + sc.BidVolume - z toho odvodis smer a market size .....
	sc.Volume[sc.Index] - limit max
	sc.HLAvg[sc.Index] - smartatctivity
	sc.OpenInterest - limit count

	s_SCPositionData Data_Pozice;
	sc.GetTradePosition(Data_Pozice);
	velikost_pozice = Data_Pozice.PositionQuantity;
	pokud je velikost zaporna mas short a kladna long

	//LOGOVANI
	mam to vyresene tak to logovani ze se vypise log jen pri zmene indexu :
	if (logovani.GetYesNo() == 1 && stary_index_persist != sc.CurrentSystemDateTime && sc.IsFullRecalculation == 0){
	stary_index_persist = sc.CurrentSystemDateTime ;

	// zde je log ..... blablabla ktery je vypisovan 1x za index a ne pri reloadu grafu

	}


*/


		//------RESENI VSTUPU--------

	if (sc.BaseDateTimeIn[sc.Index].GetTime() > zacatek_rth.GetTime() && sc.BaseDateTimeIn[sc.Index].GetTime() < konec_rth.GetTime()) 
	{
		if (long_short.GetIndex() == 0) //je li nastaven na oba smery
		{
			if (smer_limitu.GetYesNo() == 1 && sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi short definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.SellEntry(Prikaz);

				//vystup do logu
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);

			}
			else if (sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi short definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.BuyEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}

			if (smer_limitu.GetYesNo() == 1 && sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi long definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.BuyEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g,slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.BidVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}
			else if (sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi long absolutnimi relativnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.SellEntry(Prikaz);

				//vystup do logu		
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.BidVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}
		}
		else if (long_short.GetIndex() == 1) //je li nastaven jen na long 
		{
			if (smer_limitu.GetYesNo() == 0 && sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi short definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.BuyEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g,slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}

			else if (smer_limitu.GetYesNo() == 1 && sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi long definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.BuyEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.BidVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}
		}
		else if (long_short.GetIndex() == 2) //je li nastaven jen na short 		
		{
			if (smer_limitu.GetYesNo() == 1 && sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi short definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.SellEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}

			else if (smer_limitu.GetYesNo() == 0 && sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi long absolutnimi relativnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.SellEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.BidVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}
		}
	}
	//---------RESENI VYSTUPU NA KONCI DNE----------

	s_SCPositionData pozice;
	sc.GetTradePosition(pozice);
	int velikost_pozice = pozice.PositionQuantity;

	if (velikost_pozice != 0 && sc.BaseDateTimeIn[sc.Index].GetTime() >= konec_rth.GetTime()) {
		sc.FlattenAndCancelAllOrders();
	}
}

SCSFExport scsf_WMO_market_processing_analyzer_absolute_01(SCStudyInterfaceRef sc)
{
	SCInputRef velikost_marketu = sc.Input[6];
	SCInputRef velikost_limitu = sc.Input[7];
	SCInputRef pocet_limitu = sc.Input[8];
	SCInputRef zacatek_rth = sc.Input[11];
	SCInputRef konec_rth = sc.Input[12];
	SCInputRef smer_limitu = sc.Input[13];

	SCInputRef MaximumPositionAllowed = sc.Input[30];
	SCInputRef PositionQuantity = sc.Input[31];
	SCInputRef AllowEntryWithWorkingOrders = sc.Input[32];
	SCInputRef SupportReversals = sc.Input[33];
	SCInputRef SendOrdersToTradeService = sc.Input[34];
	SCInputRef AllowMultipleEntriesInSameDirection = sc.Input[35];
	SCInputRef AllowOppositeEntryWithOpposingPositionOrOrders = sc.Input[36];
	SCInputRef CancelAllOrdersOnEntriesAndReversals = sc.Input[38];
	SCInputRef CancelAllWorkingOrdersOnExit = sc.Input[39];
	SCInputRef AllowOnlyOneTradePerBar = sc.Input[40];

	SCInputRef long_short = sc.Input[43];

	if (sc.SetDefaults)
	{
		sc.GraphName = "WMO Market Processing Analyzer - Absolute 01";

		//nastaveni prahovych hodnot 
		velikost_marketu.Name = "Minimal Market Size >= ....";
		velikost_marketu.SetInt(100);

		velikost_limitu.Name = "Mininimal Max Limit >= ....";
		velikost_limitu.SetInt(0);

		pocet_limitu.Name = "Minimal Limit Order Count >= ....";
		pocet_limitu.SetInt(0);

		//nastaveni casu
		zacatek_rth.Name = "Trade From:";
		zacatek_rth.SetTime(HMS_TIME(8, 30, 0));

		konec_rth.Name = "Trade Till:";
		konec_rth.SetTime(HMS_TIME(14, 55, 0));

		//nastaveni objemu 
		PositionQuantity.Name = "Position Quantity";
		PositionQuantity.SetInt(6);

		MaximumPositionAllowed.Name = "Maximum Position Allowed";
		MaximumPositionAllowed.SetInt(6);

		//nastaveni boolu 
		smer_limitu.Name = "Limit Direction (Yes), Market Direction (No) ";
		smer_limitu.SetYesNo(true);

		AllowEntryWithWorkingOrders.Name = "Allow Entry With Working Orders (Yes/No)?";
		AllowEntryWithWorkingOrders.SetYesNo(false);

		SupportReversals.Name = "Support Reversals (Yes/No)?";
		SupportReversals.SetYesNo(false);

		SendOrdersToTradeService.Name = "Send Orders To Trade Service (Yes/No)?";
		SendOrdersToTradeService.SetYesNo(false);

		AllowMultipleEntriesInSameDirection.Name = "Allow Multiple Entries In Same Direction (Yes/No)";
		AllowMultipleEntriesInSameDirection.SetYesNo(false);

		AllowOppositeEntryWithOpposingPositionOrOrders.Name = "Allow Opposite Entry With Opposing Position Or Orders (Yes/No)";
		AllowOppositeEntryWithOpposingPositionOrOrders.SetYesNo(false);

		CancelAllOrdersOnEntriesAndReversals.Name = "Cancel All Orders On Entries And Reversals (Yes/No)";
		CancelAllOrdersOnEntriesAndReversals.SetYesNo(false);

		CancelAllWorkingOrdersOnExit.Name = "Cancel All Working Orders On Exit (Yes/No)";
		CancelAllWorkingOrdersOnExit.SetYesNo(true);

		AllowOnlyOneTradePerBar.Name = "Allow Only One Trade Per Bar (Yes/No)";
		AllowOnlyOneTradePerBar.SetYesNo(false);

		// vyber long/short smer
		long_short.Name = "Direction";
		long_short.SetCustomInputStrings("Both;Long;Short");
		long_short.SetCustomInputIndex(0);

		sc.AutoLoop = 1;
		sc.UpdateAlways = 1;
		sc.FreeDLL = 0;

		sc.CalculationPrecedence = LOW_PREC_LEVEL;
		return;
	}

	sc.MaximumPositionAllowed = MaximumPositionAllowed.GetInt(); //ok
	sc.AllowEntryWithWorkingOrders = AllowEntryWithWorkingOrders.GetInt(); //ok
	sc.SupportReversals = SupportReversals.GetYesNo(); //ok
	sc.SendOrdersToTradeService = SendOrdersToTradeService.GetYesNo(); //ok
	sc.AllowMultipleEntriesInSameDirection = AllowMultipleEntriesInSameDirection.GetYesNo(); //ok
	sc.AllowOppositeEntryWithOpposingPositionOrOrders = AllowOppositeEntryWithOpposingPositionOrOrders.GetYesNo(); //ok
	sc.SupportAttachedOrdersForTrading = true; //ok
	sc.CancelAllOrdersOnEntriesAndReversals = CancelAllOrdersOnEntriesAndReversals.GetYesNo(); //ok;
	sc.CancelAllWorkingOrdersOnExit = CancelAllWorkingOrdersOnExit.GetYesNo(); //ok
	sc.AllowOnlyOneTradePerBar = AllowOnlyOneTradePerBar.GetYesNo(); //ok
	sc.MaintainTradeStatisticsAndTradesData = true;

	/*	sc.AskVolume + sc.BidVolume - z toho odvodis smer a market size .....
	sc.Volume[sc.Index] - limit max
	sc.HLAvg[sc.Index] - smartatctivity
	sc.OpenInterest - limit count

	s_SCPositionData Data_Pozice;
	sc.GetTradePosition(Data_Pozice);
	velikost_pozice = Data_Pozice.PositionQuantity;
	pokud je velikost zaporna mas short a kladna long

	//LOGOVANI
	mam to vyresene tak to logovani ze se vypise log jen pri zmene indexu :
	if (logovani.GetYesNo() == 1 && stary_index_persist != sc.CurrentSystemDateTime && sc.IsFullRecalculation == 0){
	stary_index_persist = sc.CurrentSystemDateTime ;

	// zde je log ..... blablabla ktery je vypisovan 1x za index a ne pri reloadu grafu

	}


	*/


	//------RESENI VSTUPU--------

	if (sc.BaseDateTimeIn[sc.Index].GetTime() > zacatek_rth.GetTime() && sc.BaseDateTimeIn[sc.Index].GetTime() < konec_rth.GetTime())
	{
		if (long_short.GetIndex() == 0) //je li nastaven na oba smery
		{
			if (smer_limitu.GetYesNo() == 1 && sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi short definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.SellEntry(Prikaz);

				//vystup do logu
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);

			}
			else if (sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi short definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.BuyEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}

			if (smer_limitu.GetYesNo() == 1 && sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi long definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.BuyEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g,slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.BidVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}
			else if (sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi long absolutnimi relativnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.SellEntry(Prikaz);

				//vystup do logu		
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.BidVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}
		}
		else if (long_short.GetIndex() == 1) //je li nastaven jen na long 
		{
			if (smer_limitu.GetYesNo() == 0 && sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi short definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.BuyEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g,slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}

			else if (smer_limitu.GetYesNo() == 1 && sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi long definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.BuyEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.BidVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}
		}
		else if (long_short.GetIndex() == 2) //je li nastaven jen na short 		
		{
			if (smer_limitu.GetYesNo() == 1 && sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi short definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.SellEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}

			else if (smer_limitu.GetYesNo() == 0 && sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi long absolutnimi relativnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.SellEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.BidVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}
		}
	}
	//---------RESENI VYSTUPU NA KONCI DNE----------

	s_SCPositionData pozice;
	sc.GetTradePosition(pozice);
	int velikost_pozice = pozice.PositionQuantity;

	if (velikost_pozice != 0 && sc.BaseDateTimeIn[sc.Index].GetTime() >= konec_rth.GetTime()) {
		sc.FlattenAndCancelAllOrders();
	}
}


SCSFExport scsf_WMO_market_processing_analyzer_absolute02(SCStudyInterfaceRef sc)
{
	SCInputRef velikost_marketu = sc.Input[6];
	SCInputRef velikost_limitu = sc.Input[7];
	SCInputRef pocet_limitu = sc.Input[8];
	SCInputRef zacatek_rth = sc.Input[11];
	SCInputRef konec_rth = sc.Input[12];
	SCInputRef smer_limitu = sc.Input[13];

	SCInputRef MaximumPositionAllowed = sc.Input[30];
	SCInputRef PositionQuantity = sc.Input[31];
	SCInputRef AllowEntryWithWorkingOrders = sc.Input[32];
	SCInputRef SupportReversals = sc.Input[33];
	SCInputRef SendOrdersToTradeService = sc.Input[34];
	SCInputRef AllowMultipleEntriesInSameDirection = sc.Input[35];
	SCInputRef AllowOppositeEntryWithOpposingPositionOrOrders = sc.Input[36];
	SCInputRef CancelAllOrdersOnEntriesAndReversals = sc.Input[38];
	SCInputRef CancelAllWorkingOrdersOnExit = sc.Input[39];
	SCInputRef AllowOnlyOneTradePerBar = sc.Input[40];

	SCInputRef long_short = sc.Input[43];

	if (sc.SetDefaults)
	{
		sc.GraphName = "WMO Market Processing Analyzer - Absolute 02";

		//nastaveni prahovych hodnot 
		velikost_marketu.Name = "Minimal Market Size >= ....";
		velikost_marketu.SetInt(100);

		velikost_limitu.Name = "Mininimal Max Limit >= ....";
		velikost_limitu.SetInt(0);

		pocet_limitu.Name = "Minimal Limit Order Count >= ....";
		pocet_limitu.SetInt(0);

		//nastaveni casu
		zacatek_rth.Name = "Trade From:";
		zacatek_rth.SetTime(HMS_TIME(8, 30, 0));

		konec_rth.Name = "Trade Till:";
		konec_rth.SetTime(HMS_TIME(14, 55, 0));

		//nastaveni objemu 
		PositionQuantity.Name = "Position Quantity";
		PositionQuantity.SetInt(6);

		MaximumPositionAllowed.Name = "Maximum Position Allowed";
		MaximumPositionAllowed.SetInt(6);

		//nastaveni boolu 
		smer_limitu.Name = "Limit Direction (Yes), Market Direction (No) ";
		smer_limitu.SetYesNo(true);

		AllowEntryWithWorkingOrders.Name = "Allow Entry With Working Orders (Yes/No)?";
		AllowEntryWithWorkingOrders.SetYesNo(false);

		SupportReversals.Name = "Support Reversals (Yes/No)?";
		SupportReversals.SetYesNo(false);

		SendOrdersToTradeService.Name = "Send Orders To Trade Service (Yes/No)?";
		SendOrdersToTradeService.SetYesNo(false);

		AllowMultipleEntriesInSameDirection.Name = "Allow Multiple Entries In Same Direction (Yes/No)";
		AllowMultipleEntriesInSameDirection.SetYesNo(false);

		AllowOppositeEntryWithOpposingPositionOrOrders.Name = "Allow Opposite Entry With Opposing Position Or Orders (Yes/No)";
		AllowOppositeEntryWithOpposingPositionOrOrders.SetYesNo(false);

		CancelAllOrdersOnEntriesAndReversals.Name = "Cancel All Orders On Entries And Reversals (Yes/No)";
		CancelAllOrdersOnEntriesAndReversals.SetYesNo(false);

		CancelAllWorkingOrdersOnExit.Name = "Cancel All Working Orders On Exit (Yes/No)";
		CancelAllWorkingOrdersOnExit.SetYesNo(true);

		AllowOnlyOneTradePerBar.Name = "Allow Only One Trade Per Bar (Yes/No)";
		AllowOnlyOneTradePerBar.SetYesNo(false);

		// vyber long/short smer
		long_short.Name = "Direction";
		long_short.SetCustomInputStrings("Both;Long;Short");
		long_short.SetCustomInputIndex(0);

		sc.AutoLoop = 1;
		sc.UpdateAlways = 1;
		sc.FreeDLL = 0;

		sc.CalculationPrecedence = LOW_PREC_LEVEL;
		return;
	}

	sc.MaximumPositionAllowed = MaximumPositionAllowed.GetInt(); //ok
	sc.AllowEntryWithWorkingOrders = AllowEntryWithWorkingOrders.GetInt(); //ok
	sc.SupportReversals = SupportReversals.GetYesNo(); //ok
	sc.SendOrdersToTradeService = SendOrdersToTradeService.GetYesNo(); //ok
	sc.AllowMultipleEntriesInSameDirection = AllowMultipleEntriesInSameDirection.GetYesNo(); //ok
	sc.AllowOppositeEntryWithOpposingPositionOrOrders = AllowOppositeEntryWithOpposingPositionOrOrders.GetYesNo(); //ok
	sc.SupportAttachedOrdersForTrading = true; //ok
	sc.CancelAllOrdersOnEntriesAndReversals = CancelAllOrdersOnEntriesAndReversals.GetYesNo(); //ok;
	sc.CancelAllWorkingOrdersOnExit = CancelAllWorkingOrdersOnExit.GetYesNo(); //ok
	sc.AllowOnlyOneTradePerBar = AllowOnlyOneTradePerBar.GetYesNo(); //ok
	sc.MaintainTradeStatisticsAndTradesData = true;

	/*	sc.AskVolume + sc.BidVolume - z toho odvodis smer a market size .....
	sc.Volume[sc.Index] - limit max
	sc.HLAvg[sc.Index] - smartatctivity
	sc.OpenInterest - limit count

	s_SCPositionData Data_Pozice;
	sc.GetTradePosition(Data_Pozice);
	velikost_pozice = Data_Pozice.PositionQuantity;
	pokud je velikost zaporna mas short a kladna long

	//LOGOVANI
	mam to vyresene tak to logovani ze se vypise log jen pri zmene indexu :
	if (logovani.GetYesNo() == 1 && stary_index_persist != sc.CurrentSystemDateTime && sc.IsFullRecalculation == 0){
	stary_index_persist = sc.CurrentSystemDateTime ;

	// zde je log ..... blablabla ktery je vypisovan 1x za index a ne pri reloadu grafu

	}


	*/


	//------RESENI VSTUPU--------

	if (sc.BaseDateTimeIn[sc.Index].GetTime() > zacatek_rth.GetTime() && sc.BaseDateTimeIn[sc.Index].GetTime() < konec_rth.GetTime())
	{
		if (long_short.GetIndex() == 0) //je li nastaven na oba smery
		{
			if (smer_limitu.GetYesNo() == 1 && sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi short definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.SellEntry(Prikaz);

				//vystup do logu
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);

			}
			else if (sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi short definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.BuyEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}

			if (smer_limitu.GetYesNo() == 1 && sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi long definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.BuyEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g,slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.BidVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}
			else if (sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi long absolutnimi relativnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.SellEntry(Prikaz);

				//vystup do logu		
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.BidVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}
		}
		else if (long_short.GetIndex() == 1) //je li nastaven jen na long 
		{
			if (smer_limitu.GetYesNo() == 0 && sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi short definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.BuyEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g,slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}

			else if (smer_limitu.GetYesNo() == 1 && sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi long definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.BuyEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.BidVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}
		}
		else if (long_short.GetIndex() == 2) //je li nastaven jen na short 		
		{
			if (smer_limitu.GetYesNo() == 1 && sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi short definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.SellEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}

			else if (smer_limitu.GetYesNo() == 0 && sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi long absolutnimi relativnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.SellEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.BidVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}
		}
	}
	//---------RESENI VYSTUPU NA KONCI DNE----------

	s_SCPositionData pozice;
	sc.GetTradePosition(pozice);
	int velikost_pozice = pozice.PositionQuantity;

	if (velikost_pozice != 0 && sc.BaseDateTimeIn[sc.Index].GetTime() >= konec_rth.GetTime()) {
		sc.FlattenAndCancelAllOrders();
	}
}


SCSFExport scsf_WMO_market_processing_analyzer_absolute03(SCStudyInterfaceRef sc)
{
	SCInputRef velikost_marketu = sc.Input[6];
	SCInputRef velikost_limitu = sc.Input[7];
	SCInputRef pocet_limitu = sc.Input[8];
	SCInputRef zacatek_rth = sc.Input[11];
	SCInputRef konec_rth = sc.Input[12];
	SCInputRef smer_limitu = sc.Input[13];

	SCInputRef MaximumPositionAllowed = sc.Input[30];
	SCInputRef PositionQuantity = sc.Input[31];
	SCInputRef AllowEntryWithWorkingOrders = sc.Input[32];
	SCInputRef SupportReversals = sc.Input[33];
	SCInputRef SendOrdersToTradeService = sc.Input[34];
	SCInputRef AllowMultipleEntriesInSameDirection = sc.Input[35];
	SCInputRef AllowOppositeEntryWithOpposingPositionOrOrders = sc.Input[36];
	SCInputRef CancelAllOrdersOnEntriesAndReversals = sc.Input[38];
	SCInputRef CancelAllWorkingOrdersOnExit = sc.Input[39];
	SCInputRef AllowOnlyOneTradePerBar = sc.Input[40];

	SCInputRef long_short = sc.Input[43];

	if (sc.SetDefaults)
	{
		sc.GraphName = "WMO Market Processing Analyzer - Absolute 03";

		//nastaveni prahovych hodnot 
		velikost_marketu.Name = "Minimal Market Size >= ....";
		velikost_marketu.SetInt(100);

		velikost_limitu.Name = "Mininimal Max Limit >= ....";
		velikost_limitu.SetInt(0);

		pocet_limitu.Name = "Minimal Limit Order Count >= ....";
		pocet_limitu.SetInt(0);

		//nastaveni casu
		zacatek_rth.Name = "Trade From:";
		zacatek_rth.SetTime(HMS_TIME(8, 30, 0));

		konec_rth.Name = "Trade Till:";
		konec_rth.SetTime(HMS_TIME(14, 55, 0));

		//nastaveni objemu 
		PositionQuantity.Name = "Position Quantity";
		PositionQuantity.SetInt(6);

		MaximumPositionAllowed.Name = "Maximum Position Allowed";
		MaximumPositionAllowed.SetInt(6);

		//nastaveni boolu 
		smer_limitu.Name = "Limit Direction (Yes), Market Direction (No) ";
		smer_limitu.SetYesNo(true);

		AllowEntryWithWorkingOrders.Name = "Allow Entry With Working Orders (Yes/No)?";
		AllowEntryWithWorkingOrders.SetYesNo(false);

		SupportReversals.Name = "Support Reversals (Yes/No)?";
		SupportReversals.SetYesNo(false);

		SendOrdersToTradeService.Name = "Send Orders To Trade Service (Yes/No)?";
		SendOrdersToTradeService.SetYesNo(false);

		AllowMultipleEntriesInSameDirection.Name = "Allow Multiple Entries In Same Direction (Yes/No)";
		AllowMultipleEntriesInSameDirection.SetYesNo(false);

		AllowOppositeEntryWithOpposingPositionOrOrders.Name = "Allow Opposite Entry With Opposing Position Or Orders (Yes/No)";
		AllowOppositeEntryWithOpposingPositionOrOrders.SetYesNo(false);

		CancelAllOrdersOnEntriesAndReversals.Name = "Cancel All Orders On Entries And Reversals (Yes/No)";
		CancelAllOrdersOnEntriesAndReversals.SetYesNo(false);

		CancelAllWorkingOrdersOnExit.Name = "Cancel All Working Orders On Exit (Yes/No)";
		CancelAllWorkingOrdersOnExit.SetYesNo(true);

		AllowOnlyOneTradePerBar.Name = "Allow Only One Trade Per Bar (Yes/No)";
		AllowOnlyOneTradePerBar.SetYesNo(false);

		// vyber long/short smer
		long_short.Name = "Direction";
		long_short.SetCustomInputStrings("Both;Long;Short");
		long_short.SetCustomInputIndex(0);

		sc.AutoLoop = 1;
		sc.UpdateAlways = 1;
		sc.FreeDLL = 0;

		sc.CalculationPrecedence = LOW_PREC_LEVEL;
		return;
	}

	sc.MaximumPositionAllowed = MaximumPositionAllowed.GetInt(); //ok
	sc.AllowEntryWithWorkingOrders = AllowEntryWithWorkingOrders.GetInt(); //ok
	sc.SupportReversals = SupportReversals.GetYesNo(); //ok
	sc.SendOrdersToTradeService = SendOrdersToTradeService.GetYesNo(); //ok
	sc.AllowMultipleEntriesInSameDirection = AllowMultipleEntriesInSameDirection.GetYesNo(); //ok
	sc.AllowOppositeEntryWithOpposingPositionOrOrders = AllowOppositeEntryWithOpposingPositionOrOrders.GetYesNo(); //ok
	sc.SupportAttachedOrdersForTrading = true; //ok
	sc.CancelAllOrdersOnEntriesAndReversals = CancelAllOrdersOnEntriesAndReversals.GetYesNo(); //ok;
	sc.CancelAllWorkingOrdersOnExit = CancelAllWorkingOrdersOnExit.GetYesNo(); //ok
	sc.AllowOnlyOneTradePerBar = AllowOnlyOneTradePerBar.GetYesNo(); //ok
	sc.MaintainTradeStatisticsAndTradesData = true;

	/*	sc.AskVolume + sc.BidVolume - z toho odvodis smer a market size .....
	sc.Volume[sc.Index] - limit max
	sc.HLAvg[sc.Index] - smartatctivity
	sc.OpenInterest - limit count

	s_SCPositionData Data_Pozice;
	sc.GetTradePosition(Data_Pozice);
	velikost_pozice = Data_Pozice.PositionQuantity;
	pokud je velikost zaporna mas short a kladna long

	//LOGOVANI
	mam to vyresene tak to logovani ze se vypise log jen pri zmene indexu :
	if (logovani.GetYesNo() == 1 && stary_index_persist != sc.CurrentSystemDateTime && sc.IsFullRecalculation == 0){
	stary_index_persist = sc.CurrentSystemDateTime ;

	// zde je log ..... blablabla ktery je vypisovan 1x za index a ne pri reloadu grafu

	}


	*/


	//------RESENI VSTUPU--------

	if (sc.BaseDateTimeIn[sc.Index].GetTime() > zacatek_rth.GetTime() && sc.BaseDateTimeIn[sc.Index].GetTime() < konec_rth.GetTime())
	{
		if (long_short.GetIndex() == 0) //je li nastaven na oba smery
		{
			if (smer_limitu.GetYesNo() == 1 && sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi short definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.SellEntry(Prikaz);

				//vystup do logu
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);

			}
			else if (sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi short definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.BuyEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}

			if (smer_limitu.GetYesNo() == 1 && sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi long definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.BuyEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g,slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.BidVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}
			else if (sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi long absolutnimi relativnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.SellEntry(Prikaz);

				//vystup do logu		
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.BidVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}
		}
		else if (long_short.GetIndex() == 1) //je li nastaven jen na long 
		{
			if (smer_limitu.GetYesNo() == 0 && sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi short definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.BuyEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g,slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}

			else if (smer_limitu.GetYesNo() == 1 && sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi long definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.BuyEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.BidVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}
		}
		else if (long_short.GetIndex() == 2) //je li nastaven jen na short 		
		{
			if (smer_limitu.GetYesNo() == 1 && sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi short definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.SellEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}

			else if (smer_limitu.GetYesNo() == 0 && sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi long absolutnimi relativnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.SellEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.BidVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}
		}
	}
	//---------RESENI VYSTUPU NA KONCI DNE----------

	s_SCPositionData pozice;
	sc.GetTradePosition(pozice);
	int velikost_pozice = pozice.PositionQuantity;

	if (velikost_pozice != 0 && sc.BaseDateTimeIn[sc.Index].GetTime() >= konec_rth.GetTime()) {
		sc.FlattenAndCancelAllOrders();
	}
}


SCSFExport scsf_WMO_market_processing_analyzer_absolute04(SCStudyInterfaceRef sc)
{
	SCInputRef velikost_marketu = sc.Input[6];
	SCInputRef velikost_limitu = sc.Input[7];
	SCInputRef pocet_limitu = sc.Input[8];
	SCInputRef zacatek_rth = sc.Input[11];
	SCInputRef konec_rth = sc.Input[12];
	SCInputRef smer_limitu = sc.Input[13];

	SCInputRef MaximumPositionAllowed = sc.Input[30];
	SCInputRef PositionQuantity = sc.Input[31];
	SCInputRef AllowEntryWithWorkingOrders = sc.Input[32];
	SCInputRef SupportReversals = sc.Input[33];
	SCInputRef SendOrdersToTradeService = sc.Input[34];
	SCInputRef AllowMultipleEntriesInSameDirection = sc.Input[35];
	SCInputRef AllowOppositeEntryWithOpposingPositionOrOrders = sc.Input[36];
	SCInputRef CancelAllOrdersOnEntriesAndReversals = sc.Input[38];
	SCInputRef CancelAllWorkingOrdersOnExit = sc.Input[39];
	SCInputRef AllowOnlyOneTradePerBar = sc.Input[40];

	SCInputRef long_short = sc.Input[43];

	if (sc.SetDefaults)
	{
		sc.GraphName = "WMO Market Processing Analyzer - Absolute 04";

		//nastaveni prahovych hodnot 
		velikost_marketu.Name = "Minimal Market Size >= ....";
		velikost_marketu.SetInt(100);

		velikost_limitu.Name = "Mininimal Max Limit >= ....";
		velikost_limitu.SetInt(0);

		pocet_limitu.Name = "Minimal Limit Order Count >= ....";
		pocet_limitu.SetInt(0);

		//nastaveni casu
		zacatek_rth.Name = "Trade From:";
		zacatek_rth.SetTime(HMS_TIME(8, 30, 0));

		konec_rth.Name = "Trade Till:";
		konec_rth.SetTime(HMS_TIME(14, 55, 0));

		//nastaveni objemu 
		PositionQuantity.Name = "Position Quantity";
		PositionQuantity.SetInt(6);

		MaximumPositionAllowed.Name = "Maximum Position Allowed";
		MaximumPositionAllowed.SetInt(6);

		//nastaveni boolu 
		smer_limitu.Name = "Limit Direction (Yes), Market Direction (No) ";
		smer_limitu.SetYesNo(true);

		AllowEntryWithWorkingOrders.Name = "Allow Entry With Working Orders (Yes/No)?";
		AllowEntryWithWorkingOrders.SetYesNo(false);

		SupportReversals.Name = "Support Reversals (Yes/No)?";
		SupportReversals.SetYesNo(false);

		SendOrdersToTradeService.Name = "Send Orders To Trade Service (Yes/No)?";
		SendOrdersToTradeService.SetYesNo(false);

		AllowMultipleEntriesInSameDirection.Name = "Allow Multiple Entries In Same Direction (Yes/No)";
		AllowMultipleEntriesInSameDirection.SetYesNo(false);

		AllowOppositeEntryWithOpposingPositionOrOrders.Name = "Allow Opposite Entry With Opposing Position Or Orders (Yes/No)";
		AllowOppositeEntryWithOpposingPositionOrOrders.SetYesNo(false);

		CancelAllOrdersOnEntriesAndReversals.Name = "Cancel All Orders On Entries And Reversals (Yes/No)";
		CancelAllOrdersOnEntriesAndReversals.SetYesNo(false);

		CancelAllWorkingOrdersOnExit.Name = "Cancel All Working Orders On Exit (Yes/No)";
		CancelAllWorkingOrdersOnExit.SetYesNo(true);

		AllowOnlyOneTradePerBar.Name = "Allow Only One Trade Per Bar (Yes/No)";
		AllowOnlyOneTradePerBar.SetYesNo(false);

		// vyber long/short smer
		long_short.Name = "Direction";
		long_short.SetCustomInputStrings("Both;Long;Short");
		long_short.SetCustomInputIndex(0);

		sc.AutoLoop = 1;
		sc.UpdateAlways = 1;
		sc.FreeDLL = 0;

		sc.CalculationPrecedence = LOW_PREC_LEVEL;
		return;
	}

	sc.MaximumPositionAllowed = MaximumPositionAllowed.GetInt(); //ok
	sc.AllowEntryWithWorkingOrders = AllowEntryWithWorkingOrders.GetInt(); //ok
	sc.SupportReversals = SupportReversals.GetYesNo(); //ok
	sc.SendOrdersToTradeService = SendOrdersToTradeService.GetYesNo(); //ok
	sc.AllowMultipleEntriesInSameDirection = AllowMultipleEntriesInSameDirection.GetYesNo(); //ok
	sc.AllowOppositeEntryWithOpposingPositionOrOrders = AllowOppositeEntryWithOpposingPositionOrOrders.GetYesNo(); //ok
	sc.SupportAttachedOrdersForTrading = true; //ok
	sc.CancelAllOrdersOnEntriesAndReversals = CancelAllOrdersOnEntriesAndReversals.GetYesNo(); //ok;
	sc.CancelAllWorkingOrdersOnExit = CancelAllWorkingOrdersOnExit.GetYesNo(); //ok
	sc.AllowOnlyOneTradePerBar = AllowOnlyOneTradePerBar.GetYesNo(); //ok
	sc.MaintainTradeStatisticsAndTradesData = true;

	/*	sc.AskVolume + sc.BidVolume - z toho odvodis smer a market size .....
	sc.Volume[sc.Index] - limit max
	sc.HLAvg[sc.Index] - smartatctivity
	sc.OpenInterest - limit count

	s_SCPositionData Data_Pozice;
	sc.GetTradePosition(Data_Pozice);
	velikost_pozice = Data_Pozice.PositionQuantity;
	pokud je velikost zaporna mas short a kladna long

	//LOGOVANI
	mam to vyresene tak to logovani ze se vypise log jen pri zmene indexu :
	if (logovani.GetYesNo() == 1 && stary_index_persist != sc.CurrentSystemDateTime && sc.IsFullRecalculation == 0){
	stary_index_persist = sc.CurrentSystemDateTime ;

	// zde je log ..... blablabla ktery je vypisovan 1x za index a ne pri reloadu grafu

	}


	*/


	//------RESENI VSTUPU--------

	if (sc.BaseDateTimeIn[sc.Index].GetTime() > zacatek_rth.GetTime() && sc.BaseDateTimeIn[sc.Index].GetTime() < konec_rth.GetTime())
	{
		if (long_short.GetIndex() == 0) //je li nastaven na oba smery
		{
			if (smer_limitu.GetYesNo() == 1 && sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi short definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.SellEntry(Prikaz);

				//vystup do logu
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);

			}
			else if (sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi short definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.BuyEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}

			if (smer_limitu.GetYesNo() == 1 && sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi long definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.BuyEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g,slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.BidVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}
			else if (sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi long absolutnimi relativnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.SellEntry(Prikaz);

				//vystup do logu		
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.BidVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}
		}
		else if (long_short.GetIndex() == 1) //je li nastaven jen na long 
		{
			if (smer_limitu.GetYesNo() == 0 && sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi short definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.BuyEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g,slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}

			else if (smer_limitu.GetYesNo() == 1 && sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi long definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.BuyEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.BidVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}
		}
		else if (long_short.GetIndex() == 2) //je li nastaven jen na short 		
		{
			if (smer_limitu.GetYesNo() == 1 && sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi short definovany absolutnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.SellEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}

			else if (smer_limitu.GetYesNo() == 0 && sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= velikost_limitu.GetInt() && sc.OpenInterest[sc.Index] >= pocet_limitu.GetInt()) { //vezmi long absolutnimi relativnimi hodnotami 
				s_SCNewOrder Prikaz;
				Prikaz.OrderQuantity = PositionQuantity.GetInt();
				Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
				sc.SellEntry(Prikaz);

				//vystup do logu	
				SCString parametry_vstupu;
				parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.BidVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
				sc.AddMessageToLog(parametry_vstupu, 0);
			}
		}
	}
	//---------RESENI VYSTUPU NA KONCI DNE----------

	s_SCPositionData pozice;
	sc.GetTradePosition(pozice);
	int velikost_pozice = pozice.PositionQuantity;

	if (velikost_pozice != 0 && sc.BaseDateTimeIn[sc.Index].GetTime() >= konec_rth.GetTime()) {
		sc.FlattenAndCancelAllOrders();
	}
}




	SCSFExport scsf_WMO_market_processing_analyzer_relative(SCStudyInterfaceRef sc)
	{
		SCInputRef velikost_marketu = sc.Input[6];
		SCInputRef velikost_limitu = sc.Input[7];
		SCInputRef pocet_limitu = sc.Input[8];

		SCInputRef zacatek_rth = sc.Input[11];
		SCInputRef konec_rth = sc.Input[12];

		SCInputRef smer_limitu = sc.Input[13];

		SCInputRef pomer_marketu_k_maxlimitu = sc.Input[19];
		SCInputRef pomer_marketu_k_limitcount = sc.Input[20];

		SCInputRef MaximumPositionAllowed = sc.Input[30];
		SCInputRef PositionQuantity = sc.Input[31];
		SCInputRef AllowEntryWithWorkingOrders = sc.Input[32];
		SCInputRef SupportReversals = sc.Input[33];
		SCInputRef SendOrdersToTradeService = sc.Input[34];
		SCInputRef AllowMultipleEntriesInSameDirection = sc.Input[35];
		SCInputRef AllowOppositeEntryWithOpposingPositionOrOrders = sc.Input[36];
		SCInputRef CancelAllOrdersOnEntriesAndReversals = sc.Input[38];
		SCInputRef CancelAllWorkingOrdersOnExit = sc.Input[39];
		SCInputRef AllowOnlyOneTradePerBar = sc.Input[40];

		SCInputRef long_short = sc.Input[43];

		SCInputRef logovani = sc.Input[44];

		if (sc.SetDefaults)
		{
			sc.GraphName = "WMO Market Processing Analyzer - Relative";
			
			//nastaveni casu
			zacatek_rth.Name = "Trade From:";
			zacatek_rth.SetTime(HMS_TIME(8, 30, 0));

			konec_rth.Name = "Trade Till:";
			konec_rth.SetTime(HMS_TIME(14, 55, 0));

			//nastaveni prahovych hodnot 
			velikost_marketu.Name = "Minimal Market Size >= ....";
			velikost_marketu.SetInt(100);

			//nastaveni objemu 
			PositionQuantity.Name = "Position Quantity";
			PositionQuantity.SetInt(6);

			MaximumPositionAllowed.Name = "Maximum Position Allowed";
			MaximumPositionAllowed.SetInt(6);

			//nastaveni boolu 
			smer_limitu.Name = "Limit Direction (Yes), Market Direction (No) ";
			smer_limitu.SetYesNo(true);

			//nastaveni boolu 
			smer_limitu.Name = "Limit Direction (Yes), Market Direction (No) ";
			smer_limitu.SetYesNo(true);

			AllowEntryWithWorkingOrders.Name = "Allow Entry With Working Orders (Yes/No)?";
			AllowEntryWithWorkingOrders.SetYesNo(false);

			SupportReversals.Name = "Support Reversals (Yes/No)?";
			SupportReversals.SetYesNo(false);

			SendOrdersToTradeService.Name = "Send Orders To Trade Service (Yes/No)?";
			SendOrdersToTradeService.SetYesNo(false);

			AllowMultipleEntriesInSameDirection.Name = "Allow Multiple Entries In Same Direction (Yes/No)";
			AllowMultipleEntriesInSameDirection.SetYesNo(false);

			AllowOppositeEntryWithOpposingPositionOrOrders.Name = "Allow Opposite Entry With Opposing Position Or Orders (Yes/No)";
			AllowOppositeEntryWithOpposingPositionOrOrders.SetYesNo(false);

			CancelAllOrdersOnEntriesAndReversals.Name = "Cancel All Orders On Entries And Reversals (Yes/No)";
			CancelAllOrdersOnEntriesAndReversals.SetYesNo(false);

			CancelAllWorkingOrdersOnExit.Name = "Cancel All Working Orders On Exit (Yes/No)";
			CancelAllWorkingOrdersOnExit.SetYesNo(true);

			AllowOnlyOneTradePerBar.Name = "Allow Only One Trade Per Bar (Yes/No)";
			AllowOnlyOneTradePerBar.SetYesNo(false);

			// nastaveni pomeru
			pomer_marketu_k_maxlimitu.Name = "Market Size VS Max Limit";
			pomer_marketu_k_maxlimitu.SetFloat(0.0f);
			pomer_marketu_k_maxlimitu.SetFloatLimits(0.0f, 1.0f);

			pomer_marketu_k_limitcount.Name = "Market Size VS Limit Count";
			pomer_marketu_k_limitcount.SetFloat(0.0f);
			pomer_marketu_k_limitcount.SetFloatLimits(0.0f, 1.0f);

			// vyber long short
			long_short.Name = "Direction";
			long_short.SetCustomInputStrings("Both;Long;Short");
			long_short.SetCustomInputIndex(0);

			//logovani
			logovani.Name = "Vypis do logu?";
			logovani.SetYesNo(1);

			sc.AutoLoop = 1;
			sc.UpdateAlways = 1;
			sc.FreeDLL = 0;

			sc.CalculationPrecedence = LOW_PREC_LEVEL;

			return;
		}

		/*
		sc.AskVolume + sc.BidVolume - z toho odvodis smer a market size .....
		sc.Volume[sc.Index] - limit max
		sc.HLAvg[sc.Index] - smartatctivity
		sc.OpenInterest - limit count

		s_SCPositionData Data_Pozice;
		sc.GetTradePosition(Data_Pozice);
		velikost_pozice = Data_Pozice.PositionQuantity;
		pokud je velikost zaporna mas short a kladna long

		//LOGOVANI
		mam to vyresene tak to logovani ze se vypise log jen pri zmene indexu :
		if (logovani.GetYesNo() == 1 && stary_index_persist != sc.CurrentSystemDateTime && sc.IsFullRecalculation == 0){
		stary_index_persist = sc.CurrentSystemDateTime ;

		// zde je log ..... blablabla ktery je vypisovan 1x za index a ne pri reloadu grafu

		}
		*/

		sc.MaximumPositionAllowed = MaximumPositionAllowed.GetInt(); //ok
		sc.AllowEntryWithWorkingOrders = AllowEntryWithWorkingOrders.GetInt(); //ok
		sc.SupportReversals = SupportReversals.GetYesNo(); //ok
		sc.SendOrdersToTradeService = SendOrdersToTradeService.GetYesNo(); //ok
		sc.AllowMultipleEntriesInSameDirection = AllowMultipleEntriesInSameDirection.GetYesNo(); //ok
		sc.AllowOppositeEntryWithOpposingPositionOrOrders = AllowOppositeEntryWithOpposingPositionOrOrders.GetYesNo(); //ok
		sc.SupportAttachedOrdersForTrading = true; //ok
		sc.CancelAllOrdersOnEntriesAndReversals = CancelAllOrdersOnEntriesAndReversals.GetYesNo(); //ok;
		sc.CancelAllWorkingOrdersOnExit = CancelAllWorkingOrdersOnExit.GetYesNo(); //ok
		sc.AllowOnlyOneTradePerBar = AllowOnlyOneTradePerBar.GetYesNo(); //ok
		sc.MaintainTradeStatisticsAndTradesData = true;
		
		//double& cas_stareho_indexu = sc.GetPersistentDouble(1); //cas je double!


		//vypis do logu na zjisteni poctu indexu - funguje v replay i v realitme, nefunguje pri skrolovani
		//int& stary_index = sc.GetPersistentInt(1);
		//
		//if (logovani.GetYesNo() == 1 && stary_index != sc.ArraySize)
		//{
		//	SCString pocet_indexu;
		//	pocet_indexu.Format("[pocet_indexu: %i]", sc.ArraySize);
		//	sc.AddMessageToLog(pocet_indexu, 1);
		//	stary_index = sc.ArraySize;
		//}
		//

		//vypis do logu na zjisteni poctu indexu - funguje pri skrolovani
		int& vstupni_index = sc.GetPersistentInt(1);

		
		//------RESENI VSTUPU--------

		if (sc.BaseDateTimeIn[sc.Index].GetTime() > zacatek_rth.GetTime() && sc.BaseDateTimeIn[sc.Index].GetTime() < konec_rth.GetTime()) 
		{
			if (long_short.GetIndex() == 0) //je li nastaven na oba smery
			{
				if (smer_limitu.GetYesNo() == 1 && sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= pomer_marketu_k_maxlimitu.GetFloat()*sc.AskVolume[sc.Index] && sc.OpenInterest[sc.Index] >= pomer_marketu_k_limitcount.GetFloat()*sc.AskVolume[sc.Index]) { //vezmi short definovany relativnimi hodnotami 
					s_SCNewOrder Prikaz;
					Prikaz.OrderQuantity = PositionQuantity.GetInt();
					Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
					sc.SellEntry(Prikaz);
					
					//vystup do logu
					if (logovani.GetYesNo() == 1 && vstupni_index != sc.ArraySize)
					{
					SCString parametry_vstupu;
					parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
					sc.AddMessageToLog(parametry_vstupu, 0);
					vstupni_index = sc.ArraySize;
					}
					
				}
				else if (smer_limitu.GetYesNo() == 0 && sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= pomer_marketu_k_maxlimitu.GetFloat()*sc.AskVolume[sc.Index] && sc.OpenInterest[sc.Index] >= pomer_marketu_k_limitcount.GetFloat()*sc.AskVolume[sc.Index]) { //vezmi long  definovany absolutnimi hodnotami 
					s_SCNewOrder Prikaz;
					Prikaz.OrderQuantity = PositionQuantity.GetInt();
					Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
					sc.BuyEntry(Prikaz);

					//vystup do logu
					if (vstupni_index != sc.ArraySize)
					{
						SCString parametry_vstupu;
						parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
						sc.AddMessageToLog(parametry_vstupu, 0);
						vstupni_index = sc.ArraySize;
					}
				}

				if (smer_limitu.GetYesNo() == 1 && sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= pomer_marketu_k_maxlimitu.GetFloat()*sc.BidVolume[sc.Index] && sc.OpenInterest[sc.Index] >= pomer_marketu_k_limitcount.GetFloat()*sc.BidVolume[sc.Index]) { //vezmi long definovany relativnimi hodnotami 
					s_SCNewOrder Prikaz;
					Prikaz.OrderQuantity = PositionQuantity.GetInt();
					Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
					sc.BuyEntry(Prikaz);

					//vystup do logu
					if (vstupni_index != sc.ArraySize)
					{
						SCString parametry_vstupu;
						parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
						sc.AddMessageToLog(parametry_vstupu, 0);
						vstupni_index = sc.ArraySize;
					}
				}
				else if (smer_limitu.GetYesNo() == 0 && sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= pomer_marketu_k_maxlimitu.GetFloat()*sc.BidVolume[sc.Index] && sc.OpenInterest[sc.Index] >= pomer_marketu_k_limitcount.GetFloat()*sc.BidVolume[sc.Index]) { //vezmi short definovany absolutnimi hodnotami 
					s_SCNewOrder Prikaz;
					Prikaz.OrderQuantity = PositionQuantity.GetInt();
					Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
					sc.SellEntry(Prikaz);

					//vystup do logu
					if (vstupni_index != sc.ArraySize)
					{
						SCString parametry_vstupu;
						parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
						sc.AddMessageToLog(parametry_vstupu, 0);
						vstupni_index = sc.ArraySize;
					}
				}
			}
			else if (long_short.GetIndex() == 1) //je li nastaven jen na long 
			{
				if (smer_limitu.GetYesNo() == 0 && sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= pomer_marketu_k_maxlimitu.GetFloat()*sc.AskVolume[sc.Index] && sc.OpenInterest[sc.Index] >= pomer_marketu_k_limitcount.GetFloat()*sc.AskVolume[sc.Index]) { //vezmi long  definovany absolutnimi hodnotami 
					s_SCNewOrder Prikaz;
					Prikaz.OrderQuantity = PositionQuantity.GetInt();
					Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
					sc.BuyEntry(Prikaz);

					//vystup do logu
					if (vstupni_index != sc.ArraySize)
					{
						SCString parametry_vstupu;
						parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
						sc.AddMessageToLog(parametry_vstupu, 0);
						vstupni_index = sc.ArraySize;
					}
				}

				else if (smer_limitu.GetYesNo() == 1 && sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= pomer_marketu_k_maxlimitu.GetFloat()*sc.BidVolume[sc.Index] && sc.OpenInterest[sc.Index] >= pomer_marketu_k_limitcount.GetFloat()*sc.BidVolume[sc.Index]) { //vezmi long definovany relativnimi hodnotami 
					s_SCNewOrder Prikaz;
					Prikaz.OrderQuantity = PositionQuantity.GetInt();
					Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
					sc.BuyEntry(Prikaz);

					//vystup do logu
					if (vstupni_index != sc.ArraySize)
					{
						SCString parametry_vstupu;
						parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
						sc.AddMessageToLog(parametry_vstupu, 0);
						vstupni_index = sc.ArraySize;
					}
				}
			}
			else if (long_short.GetIndex() == 2) //je li nastaven jen na short 	
			{
				if (smer_limitu.GetYesNo() == 1 && sc.AskVolume[sc.Index] > 0 && sc.AskVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= pomer_marketu_k_maxlimitu.GetFloat()*sc.AskVolume[sc.Index] && sc.OpenInterest[sc.Index] >= pomer_marketu_k_limitcount.GetFloat()*sc.AskVolume[sc.Index]) { //vezmi short definovany relativnimi hodnotami 
					s_SCNewOrder Prikaz;
					Prikaz.OrderQuantity = PositionQuantity.GetInt();
					Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
					sc.SellEntry(Prikaz);

					//vystup do logu
					if (vstupni_index != sc.ArraySize)
					{
						SCString parametry_vstupu;
						parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
						sc.AddMessageToLog(parametry_vstupu, 0);
						vstupni_index = sc.ArraySize;
					}
				}
				else if (smer_limitu.GetYesNo() == 0 && sc.BidVolume[sc.Index] > 0 && sc.BidVolume[sc.Index] >= velikost_marketu.GetInt() && sc.Volume[sc.Index] >= pomer_marketu_k_maxlimitu.GetFloat()*sc.BidVolume[sc.Index] && sc.OpenInterest[sc.Index] >= pomer_marketu_k_limitcount.GetFloat()*sc.BidVolume[sc.Index]) { //vezmi short definovany absolutnimi hodnotami 
					s_SCNewOrder Prikaz;
					Prikaz.OrderQuantity = PositionQuantity.GetInt();
					Prikaz.OrderType = SCT_ORDERTYPE_MARKET;
					sc.SellEntry(Prikaz);

					//vystup do logu
					if (vstupni_index != sc.ArraySize)
					{
						SCString parametry_vstupu;
						parametry_vstupu.Format("[chart: %i, studyid: %i, marketsize: %g, maxlimit: %g, limitcount: %g, slip: %g, price: %g, time: %s]", sc.ChartNumber, sc.StudyGraphInstanceID, sc.AskVolume[sc.Index], sc.Volume[sc.Index], sc.OpenInterest[sc.Index], abs(sc.OHLCAvg[sc.Index] - sc.HLCAvg[sc.Index]), sc.Close[sc.Index], sc.FormatDateTimeMS(sc.CurrentSystemDateTimeMS).GetChars());
						sc.AddMessageToLog(parametry_vstupu, 0);
						vstupni_index = sc.ArraySize;
					}
				}
			}
		}

		//---------RESENI VYSTUPU NA KONCI DNE----------

		s_SCPositionData pozice;
		sc.GetTradePosition(pozice);
		int velikost_pozice = pozice.PositionQuantity;

		if (velikost_pozice != 0 && sc.BaseDateTimeIn[sc.Index].GetTime() >= konec_rth.GetTime()) {
			sc.FlattenAndCancelAllOrders();
		}
	}


