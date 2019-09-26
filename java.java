package com.example.myapplication.processing;

import java.util.Arrays;

public class GameBackend {

    private final char BLANK = ' ', PLAYER1 ='X', PLAYER2 ='O';
    private char[][] grille;
    private int[][] WinJetonPlacement;
    private int nbr_col, nbr_ligne, GridComplete, GridAvancement;

    public GameBackend(int nbr_col, int nbr_ligne) {
        this.nbr_col = nbr_col;
        this.nbr_ligne = nbr_ligne;
        this.GridComplete = nbr_col*nbr_ligne;
        this.grille = new char[nbr_ligne][nbr_col];
        this.WinJetonPlacement = new int[4][4];
        init();
    }

    public void init(){
        this.GridAvancement = 0;
        for ( int i = 0; i < nbr_ligne; i++ ) {
            for ( int j = 0; j <  nbr_col; j++ ) {
                grille[i][j] = BLANK;
            }
        }
    }

    public int player_rotation(int col_selected, boolean rotation){
        for ( int i = nbr_ligne-1; i >= 0; i--) {
            if(grille[i][col_selected] == BLANK){
                if(rotation){
                    grille[i][col_selected] = PLAYER1;
                }else{
                    grille[i][col_selected] = PLAYER2;
                }
                GridAvancement++;
                return i;
            }
        }
        return -1;
    }

    public boolean winnerCheck(boolean rotation){
        char pion;
        if(!rotation) pion = 'O';
        else pion = 'X';

        //Horizontal Check
        for ( int j = 0; j < nbr_col-3; j++ ) {
            for ( int i = 0; i < nbr_ligne; i++ ) {
                if(this.grille[i][j] == pion && this.grille[i][j+1] == pion && this.grille[i][j+2] == pion && this.grille[i][j+3] == pion) {
                    WinJetonPlacement[0][0] = i;
                    WinJetonPlacement[0][1] = j;
                    WinJetonPlacement[1][0] = i;
                    WinJetonPlacement[1][1] = j+1;
                    WinJetonPlacement[2][0] = i;
                    WinJetonPlacement[2][1] = j+2;
                    WinJetonPlacement[3][0] = i;
                    WinJetonPlacement[3][1] = j+3;
                    return true;
                }
            }
        }
        //VerticalCheck
        for ( int i = 0; i < nbr_ligne-3; i++ ) {
            for ( int j = 0; j < nbr_col; j++ ) {
                if(this.grille[i][j] == pion && this.grille[i+1][j] == pion && this.grille[i+2][j] == pion && this.grille[i+3][j] == pion) {
                    WinJetonPlacement[0][0] = i;
                    WinJetonPlacement[0][1] = j;
                    WinJetonPlacement[1][0] = i+1;
                    WinJetonPlacement[1][1] = j;
                    WinJetonPlacement[2][0] = i+2;
                    WinJetonPlacement[2][1] = j;
                    WinJetonPlacement[3][0] = i+3;
                    WinJetonPlacement[3][1] = j;
                    return true;
                }

            }
        }
        //ascendingDiagonalCheck
        for ( int i = 3; i < nbr_ligne; i++ ) {
            for ( int j = 0; j < nbr_col-3; j++ ) {
                if(this.grille[i][j] == pion && this.grille[i-1][j+1] == pion && this.grille[i-2][j+2] == pion && this.grille[i-3][j+3] == pion) {
                    WinJetonPlacement[0][0] = i;
                    WinJetonPlacement[0][1] = j;
                    WinJetonPlacement[1][0] = i-1;
                    WinJetonPlacement[1][1] = j+1;
                    WinJetonPlacement[2][0] = i-2;
                    WinJetonPlacement[2][1] = j+2;
                    WinJetonPlacement[3][0] = i-3;
                    WinJetonPlacement[3][1] = j+3;
                    return true;
                }
            }
        }
        //descendingDiagonalCheck
        for ( int i = 3; i < nbr_ligne; i++ ) {
            for ( int j = 3; j < nbr_col; j++ ) {
                if(this.grille[i][j] == pion && this.grille[i-1][j-1] == pion && this.grille[i-2][j-2] == pion && this.grille[i-3][j-3] == pion) {
                    WinJetonPlacement[0][0] = i;
                    WinJetonPlacement[0][1] = j;
                    WinJetonPlacement[1][0] = i-1;
                    WinJetonPlacement[1][1] = j-1;
                    WinJetonPlacement[2][0] = i-2;
                    WinJetonPlacement[2][1] = j-2;
                    WinJetonPlacement[3][0] = i-3;
                    WinJetonPlacement[3][1] = j-3;
                    return true;
                }
            }
        }
        return false;
    }

    public void affichage(){
        System.out.println(Arrays.deepToString(grille).replace("], ", "]\n"));
    }

    public void RecoveryFill(int col, int row, int player){
        char pion;
        if(player == 1) pion = PLAYER1;
        else pion = PLAYER2;
        grille[row][col] = pion;
        GridAvancement++;
    }

    public boolean FullCheck(){
        if (GridAvancement == GridComplete) return true;
        return false;
    }

    public int[][] getWinJetonPlacement() {
        return WinJetonPlacement;
    }

    public int getGridAvancement() {
        return GridAvancement;
    }

    public int getGridComplete() {
        return GridComplete;
    }
}
