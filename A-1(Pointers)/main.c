
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define NUM_MOVIES 10
int **ratingsMatrix = NULL;
int numUsers = 0;

void displayMovies()
{
    FILE *file = fopen("movie_database.txt", "r");
    printf("\n");
    printf("***** Movie Database *****\n");
    printf("\n");

    if (file == NULL)
    {
        printf("Error opening file\n");
        return;
    }

    char movie[100];
    while (fgets(movie, sizeof(movie), file) != NULL)
    {
        printf("%s", movie);
    }

    fclose(file);
    printf("\n");
}

void count_no_of_user()
{
    FILE *file = fopen("user_data.txt", "r");
    if (file == NULL)
    {
        printf("Error opening file\n");
        return;
    }

    char username[100];
    numUsers = 0;
    while (fgets(username, sizeof(username), file) != NULL)
    {
        numUsers++;
    }
    fclose(file);
}

void loadRatingsFromFile()
{
    FILE *file = fopen("user_rating.txt", "r");
    if (file == NULL)
    {
        printf("Error opening file\n");
        return;
    }

    for (int i = 0; i < numUsers; i++)
    {
        for (int j = 0; j < NUM_MOVIES; j++)
        {
            if (fscanf(file, "%d", &ratingsMatrix[i][j]) != 1)
            {
                // printf("Error reading rating for user %d, movie %d\n", i + 1, j + 1);
                ratingsMatrix[i][j] = 0; // Initialize the rating to 0 if reading fails
            }
        }
    }
    fclose(file);
}

void initializeRatingsMatrix()
{
    count_no_of_user();

    int **newRatingsMatrix = (int **)malloc(numUsers * sizeof(int *));
    if (newRatingsMatrix == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for ratings matrix\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < numUsers; i++)
    {
        newRatingsMatrix[i] = (int *)malloc(NUM_MOVIES * sizeof(int));
        if (newRatingsMatrix[i] == NULL)
        {
            fprintf(stderr, "Failed to allocate memory for ratings matrix row %d\n", i);
            for (int j = 0; j < i; j++)
            {
                free(newRatingsMatrix[j]);
            }
            free(newRatingsMatrix);
            exit(EXIT_FAILURE);
        }
    }

    // Copy the old ratings matrix to the new one, if it exists
    if (ratingsMatrix != NULL)
    {
        for (int i = 0; i < numUsers - 1; i++)
        { // Copy data only for existing users
            memcpy(newRatingsMatrix[i], ratingsMatrix[i], NUM_MOVIES * sizeof(int));
            free(ratingsMatrix[i]);
        }
        free(ratingsMatrix);
    }

    ratingsMatrix = newRatingsMatrix;

    // Load existing ratings from the file
    loadRatingsFromFile();
}

void printmatrix()
{
    for (int i = 0; i < numUsers; i++)
    {
        for (int j = 0; j < NUM_MOVIES; j++)
        {
            printf("%d ", ratingsMatrix[i][j]);
        }
        printf("\n");
    }
}

void registerUser()
{
    FILE *file = fopen("user_data.txt", "r");
    if (file == NULL)
    {
        printf("Error opening file\n");
        return;
    }

    char name[100];
    printf("Enter username for registration: ");
    scanf("%s", name);

    for (int i = 0; name[i]; i++)
    {
        name[i] = tolower((unsigned char)name[i]);
    }

    char username[100];
    while (fgets(username, sizeof(username), file) != NULL)
    {                                                       // fgets() to read each line from the file
                                                            // into the username buffer until the end of the file is reached.
        size_t len = strlen(username);
        if (len > 0 && username[len - 1] == '\n')
        {
            username[len - 1] = '\0';
        }

        for (int i = 0; username[i]; i++)
        {
            username[i] = tolower((unsigned char)username[i]);
        }

        if (strcasecmp(username, name) == 0)
        {
            printf("User already exists. Please choose a different name.\n");
            fclose(file);
            return;
        }
    }

    fclose(file);

    file = fopen("user_data.txt", "a");
    if (file == NULL)
    {
        printf("Error opening file\n");
        return;
    }

    fprintf(file, "%s\n", name);
    fclose(file);

    printf("User %s registered successfully\n", name);

    // Re-initialize the ratings matrix to accommodate the new user
    initializeRatingsMatrix();
}

void writeRatingsToFile()
{
    FILE *file = fopen("user_rating.txt", "w");
    if (file == NULL)
    {
        printf("Error opening file\n");
        return;
    }

    for (int i = 0; i < numUsers; i++)
    {
        for (int j = 0; j < NUM_MOVIES; j++)
        {
            fprintf(file, "%d ", ratingsMatrix[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

void rateMovie()
{
    char inputUsername[100]; 

    printf("Enter your username: "); 
    scanf("%s", inputUsername);

    
    for (int i = 0; inputUsername[i]; i++)
    {
        inputUsername[i] = tolower((unsigned char)inputUsername[i]);
    }

    FILE *file = fopen("user_data.txt", "r"); 
    if (file == NULL) 
    {
        printf("Error opening file\n"); 
        return; 
    }

    char usernameFromFile[100]; 
    int userIndex = -1; // Index of the found user in the file. Initialized to -1 to indicate not found.
    int count = 0; // Counter to keep track of the line number (user index in the file).
    while (fgets(usernameFromFile, sizeof(usernameFromFile), file) != NULL) // Read usernames from the file line by line.
    {
        // Remove the newline character from the end of the read username, if present.
        size_t len = strlen(usernameFromFile);
        if (len > 0 && usernameFromFile[len - 1] == '\n')
        {
            usernameFromFile[len - 1] = '\0';
        }

        // Convert the username from file to lowercase for case-insensitive comparison.
        for (int i = 0; usernameFromFile[i]; i++)
        {
            usernameFromFile[i] = tolower((unsigned char)usernameFromFile[i]);
        }

        // Check if the read username matches the input username.
        if (strcasecmp(usernameFromFile, inputUsername) == 0)
        {
            userIndex = count; 
            break; 
        }
        count++; 
    }

    fclose(file); 

    if (userIndex == -1) 
    {
        printf("User not found. Please register first.\n"); 
        return; 
    }

    int movieIndex, rating;
    printf("Enter the index of the movie you want to rate (1-10): "); 
    scanf("%d", &movieIndex); 
    printf("Enter your rating for the movie (1-5): "); 
    scanf("%d", &rating); 

    
    if (movieIndex < 1 || movieIndex > NUM_MOVIES || rating < 1 || rating > 5)
    {
        printf("Invalid input. Please try again.\n"); 
        return; 
    }

    ratingsMatrix[userIndex][movieIndex - 1] = rating; 
    printf("Rating submitted successfully.\n"); 

    writeRatingsToFile(); 
}

void printFormattedMovieName(char *movieInfo)
{
    for (int i = 0; movieInfo[i] != '\0'; i++)
    {
        if (movieInfo[i] == '_')
        {
            printf(" ");
        }
        else if (movieInfo[i] == ' ')
        {
            break; // Stop printing once we reach the genre part
        }
        else
        {
            printf("%c", movieInfo[i]);
        }
    }
}

// Function to get the index of the user
int getUserIndex(char *username)
{
    FILE *file = fopen("user_data.txt", "r");
    if (file == NULL)
    {
        printf("Error opening file\n");
        return -1;
    }

    char fileUsername[100];
    int index = 0;
    while (fgets(fileUsername, sizeof(fileUsername), file) != NULL)
    {
        // Remove the newline character from the end of the read username, if present.
        size_t len = strlen(fileUsername);
        if (len > 0 && fileUsername[len - 1] == '\n')
        {
            fileUsername[len - 1] = '\0';
        }

        // Convert the file username to lowercase
        for (int i = 0; fileUsername[i]; i++)
        {
            fileUsername[i] = tolower((unsigned char)fileUsername[i]);
        }

        // Compare with the input username
        if (strcasecmp(fileUsername, username) == 0)
        {
            fclose(file);
            return index;
        }
        index++;
    }

    fclose(file);
    return -1;
}

void getMovieRecommendations()
{
    char inputUsername[100];
    printf("Enter your username: ");
    scanf("%s", inputUsername);

    // Convert the input username to lowercase for comparison
    for (int i = 0; inputUsername[i]; i++)
    {
        inputUsername[i] = tolower((unsigned char)inputUsername[i]);
    }
   
    int userIndex = getUserIndex(inputUsername);
    if (userIndex == -1)
    {
        printf("User not found. Please register first.\n");
        return;
    }

    printf("***** Recommended Movies *****\n");
    FILE *file = fopen("movie_database.txt", "r");
    if (file == NULL)
    {
        printf("Error opening file\n");
        return;
    }

    char movieLine[200]; // Assuming each line will not exceed 200 characters
    int movieIndex = 0;
    while (fgets(movieLine, sizeof(movieLine), file) != NULL)
    {
        if (ratingsMatrix[userIndex][movieIndex] == 0)
        { // Movie not rated by user
            int totalRatings = 0;
            int ratingsCount = 0;
            for (int i = 0; i < numUsers; i++)
            {
                if (ratingsMatrix[i][movieIndex] > 0)
                {
                    totalRatings += ratingsMatrix[i][movieIndex];
                    ratingsCount++;
                }
            }
            if (ratingsCount > 0)
            {
                float predictedRating = (float)totalRatings / ratingsCount;
                printFormattedMovieName(movieLine); // Call to your existing function
                printf(" - Predicted Rating: %.1f\n", predictedRating);
            }
        }
        movieIndex++;
    }

    fclose(file);
}

void exitProgram()
{
    printf("Thank you for considering our Movie Recommendation System\n");
    // freeRatingsMatrix();
    exit(0);
}

int main()
{
    int choice;

    printf("***** Movie Recommendation System *****\n");
    printf("1. Register User\n");
    printf("2. Display Movies\n");
    printf("3. Rate a Movie\n");
    printf("4. Get Movie Recommendations\n");
    printf("0. Exit\n");

    printf("Enter your choice: ");
    scanf("%d", &choice);
    while (choice != 0)
    {

        switch (choice)
        {

        case 1:
            registerUser();
            break;
        case 2:
            // printf("Display Movies\n");
            displayMovies();
            break;
        case 3:
            printf("\n");
            printf("Rate a Movie\n");
            rateMovie();
            break;
        case 4:
            printf("\n");
            printf("Get Movie Recommendations\n");
            getMovieRecommendations();
            break;
        default:
            printf("Invalid choice\n");
            break;
        }
        printf("\n");
        printf("***** Movie Recommendation System *****\n");
        printf("1. Register User\n");
        printf("2. Display Movies\n");
        printf("3. Rate a Movie\n");
        printf("4. Get Movie Recommendations\n");
        printf("0. Exit\n");
        printf("\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
    }
    exitProgram();
    return 0;
}
